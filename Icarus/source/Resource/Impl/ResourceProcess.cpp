/* ResourceProcess.cpp
Author: Jeff Kiah
Orig.Date: 06/01/2012
*/

#include "Resource/ResourceProcess.h"
#include "Event/RegisteredEvents.h"
#include "Resource/ZipFile.h"

///// VARIABLES /////

// class static vars
const string AsyncLoadEvent::sEventType("SYS_RES_ASYNCLOAD");
const string AsyncLoadDoneEvent::sEventType("SYS_RES_ASYNCLOAD_DONE");
const string AsyncInitDoneEvent::sEventType("SYS_RES_ASYNCINIT_DONE");

///// FUNCTIONS /////

// class AsyncLoadEvent

/*
void AsyncLoadEvent::buildScriptData()
{
	mScriptData.clear();
	mScriptData.push_back(AnyVarsValue("mResName", mResName));
	mScriptData.push_back(AnyVarsValue("mSourceName", mSourceName));
	mScriptDataBuilt = true;
}

AsyncLoadEvent::AsyncLoadEvent(const AnyVars &eventData) :
	ScriptableEvent(eventData)
{
	for (AnyVars::const_iterator i = eventData.begin(); i != eventData.end(); ++i) {
		try {
			if (_stricmp(i->first.c_str(), "mResName") == 0) {
				mResName = any_cast<string>(i->second);
			} else if (_stricmp(i->first.c_str(), "mSourceName") == 0) {
				mSourceName = any_cast<string>(i->second);
			}
		} catch (const boost::bad_any_cast &ex) {
			// nothing happens with a bad datatype in release build, silently ignores
			debugPrintf("AsyncLoadEvent: bad_any_cast \"%s\"\n", ex.what());
		}
	}
}
*/

// class AsyncLoadProcess

void AsyncLoadProcess::threadProc()
{
	while (!threadKilled()) {
		ThreadEventHandler &h = *((ThreadEventHandler *)mEventListener.mAsyncLoadHandler.get());
		EventPtr ePtr;
		h.mEventQueue.waitPop(ePtr);
		// condition variable causes the process to sit idle until an event is in the queue,
		// so a shutdown event could wake the thread and then exit. If load events
		// are still queued, threadKilled() returning true could also cause an exit
		if (ePtr->type() == ResCacheManager::sAsyncLoadShutdownEvent) break;

		// if it's not a shutdown event, we know it's a decompression / load event
		AsyncLoadEvent &e = *(static_cast<AsyncLoadEvent*>(ePtr.get()));

		size_t threadIndex = -1;
		// find the threadIndex in our source map, or call getNewThreadIndex if it doesn't exist yet
		ThreadIndexMap::const_iterator i = mSourceThreadIndexMap.find(e.mSourceName);
		if (i == mSourceThreadIndexMap.end()) {	// not found in the hash_map
			threadIndex = e.mSourcePtr->getNewThreadIndex();	// request a threadIndex from the ResourceSource
			mSourceThreadIndexMap[e.mSourceName] = threadIndex;	// store in the hash_map for future reference
		} else {
			threadIndex = i->second;	// found in map, get the stored threadIndex
		}
		debugPrintf("GOT THREADINDEX = %i\n\n", threadIndex); // TEMP

		bool success = false;
		CharBufferPtr dataPtr((char *)0);
		size_t size = 0;
		// threadIndex -1 means there was an error opening the file
		if (threadIndex != -1) {
			// load from source
			size = e.mSourcePtr->getResource(e.mResName, dataPtr, threadIndex);
			if (size) {
				success = true;
				e.mResource->setSizeB(size); // set the size in Resource since many init routines rely on an accurate size
				debugPrintf("%s: async load \"%S\": success=%i\n", name().c_str(), e.mResName.c_str(), success);
			}
		}
		
		// if the loading succeeded AND this resource uses thread initializer
		if (success && e.mResource->useThreadInit()) {
			// send AsyncLoadDone event to notify initialization thread to run
			EventPtr doneEventPtr(new AsyncLoadDoneEvent(e.mResName, e.mSourceName, dataPtr,
														 size, e.mResource, success));
			m_eventMgr->raiseThreadSafe(doneEventPtr);

		} else {
			// skip the init thread and just fire a AsyncInitDone event so the main thread puts it right into the staging queue
			EventPtr initEventPtr(new AsyncInitDoneEvent(e.mResName, e.mSourceName, dataPtr,
														 size, e.mResource, success));
			m_eventMgr->raiseThreadSafe(initEventPtr);
		}
	}
}

AsyncLoadProcess::AsyncLoadProcess(const string &name, const EventManagerPtr &eventMgr) :
	ThreadProcess(name), m_eventMgr(eventMgr)
{
	// register the load event
	eventMgr->registerEventType(AsyncLoadEvent::sEventType,
							 //RegEventPtr(new ScriptCallableCodeEvent<AsyncLoadEvent>(EventDataType_NotEmpty)));
							 RegEventPtr(new CodeOnlyEvent(EventDataType_NotEmpty)));
	// register the load done event
	eventMgr->registerEventType(AsyncLoadDoneEvent::sEventType,
							 RegEventPtr(new CodeOnlyEvent(EventDataType_NotEmpty)));
}

AsyncLoadProcess::~AsyncLoadProcess()
{
	killThread();	// request thread to shut down
	// send shutdown event to wake up the thread and allow it to exit incase it's idle
	m_eventMgr->trigger(ResCacheManager::sAsyncLoadShutdownEvent);
	finish();	// ensures the main thread will wait for thread to join
}

// class AsyncLoadProcess::AsyncLoadListener

AsyncLoadProcess::AsyncLoadListener::AsyncLoadListener() :
	EventListener("AsyncLoadListener"),
	mAsyncLoadHandler(new ThreadEventHandler())
{
	registerEventHandler(AsyncLoadEvent::sEventType, mAsyncLoadHandler, 1);
	registerEventHandler(ResCacheManager::sAsyncLoadShutdownEvent, mAsyncLoadHandler, 1);
}


// class AsyncInitProcess

void AsyncInitProcess::threadProc()
{
	while (!threadKilled()) {
		ThreadEventHandler &h = *((ThreadEventHandler *)mEventListener.mAsyncLoadDoneHandler.get());
		EventPtr ePtr;
		h.mEventQueue.waitPop(ePtr);
		// condition variable causes the process to sit idle until an event is in the queue,
		// so a shutdown event could wake the thread and then exit. If load events
		// are still queued, threadKilled() returning true could also cause an exit
		if (ePtr->type() == ResCacheManager::sAsyncLoadShutdownEvent) break;

		// if it's not a shutdown event, we know it's a Load Done event
		AsyncLoadDoneEvent &e = *(static_cast<AsyncLoadDoneEvent*>(ePtr.get()));

		// run the thread initialization routine
		bool success = e.mResource->onThreadInit(e.mDataPtr);
		debugPrintf("%s: async init \"%S\": success=%i\n", name().c_str(), e.mResName.c_str(), success);

		// fire the init done event which the ResCacheManager's listener will put in the staging queue
		EventPtr initEventPtr(new AsyncInitDoneEvent(e.mResName, e.mSourceName, e.mDataPtr,
													 e.mSize, e.mResource, success));
		m_eventMgr->raiseThreadSafe(initEventPtr);
	}
}

AsyncInitProcess::AsyncInitProcess(const string &name, const EventManagerPtr &eventMgr) :
	ThreadProcess(name), m_eventMgr(eventMgr)
{
	// register the init done event
	eventMgr->registerEventType(AsyncInitDoneEvent::sEventType,
								RegEventPtr(new CodeOnlyEvent(EventDataType_NotEmpty)));
}

AsyncInitProcess::~AsyncInitProcess()
{
	killThread();	// request thread to shut down
	// send shutdown event to wake up the thread and allow it to exit incase it's idle
	m_eventMgr->trigger(ResCacheManager::sAsyncLoadShutdownEvent);
	finish();	// ensures the main thread will wait for thread to join
}


// class AsyncInitProcess::AsyncInitListener

AsyncInitProcess::AsyncInitListener::AsyncInitListener() :
	EventListener("AsyncInitListener"),
	mAsyncLoadDoneHandler(new ThreadEventHandler())
{
	registerEventHandler(AsyncLoadDoneEvent::sEventType, mAsyncLoadDoneHandler, 1);
	registerEventHandler(ResCacheManager::sAsyncLoadShutdownEvent, mAsyncLoadDoneHandler, 1);
}

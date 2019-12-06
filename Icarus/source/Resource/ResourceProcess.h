/*----==== RESOURCEPROCESS.H ====----
	Author:		Jeff Kiah
	Orig.Date:	05/25/2009
	Rev.Date:	06/10/2009
-----------------------------------*/

#pragma once

#include <string>
#include "Process/ThreadProcess.h"
#include "Event/EventManager.h"

using std::string;
using std::wstring;
using boost::checked_array_deleter;

class IResourceSource;
class Resource;

typedef shared_ptr<Resource>	ResPtr;
typedef shared_ptr<char>		BufferPtr; // use checked_array_deleter<char> to ensure delete[] called

///// STRUCTURES /////

/*=====================================================================
class AsyncLoadEvent
	This event is created when tryLoad is called for a new resource to
	be loaded. This is a task picked up by ResourceLoadProcess, which
	streams the resource bytes from their source.
=====================================================================*/
class AsyncLoadEvent : public Event /*ScriptableEvent*/ {
	public:
		typedef shared_ptr<IResourceSource>		ResSourcePtr;

		///// VARIABLES /////
		static const string sEventType;

		wstring			mResName;		// the file to load from the source object
		wstring			mSourceName;	// the name of the ResourceSource
		ResSourcePtr	mSourcePtr;		// shared_ptr to the ResourceSource
		ResPtr			mResource;		// shared_ptr to the Resource object being constructed

		///// FUNCTIONS /////
		const string &	type() const { return sEventType; }
		//void	serialize(ostream &out) const {}
		//void	deserialize(istream &in) {}
		//void	buildScriptData();

		// Constructor / destructor
		explicit AsyncLoadEvent(const wstring &resName, const wstring &sourceName,
								const ResSourcePtr &sourcePtr, const ResPtr &resPtr) :
			Event(),
			//ScriptableEvent(),
			mResName(resName), mSourceName(sourceName),
			mSourcePtr(sourcePtr), mResource(resPtr)
		{}
		//explicit AsyncLoadEvent(const AnyVars &eventData);
		virtual ~AsyncLoadEvent() {}
};

/*=====================================================================
class AsyncLoadDoneEvent
	This event marks the completion of resource streaming from source.
	This is handled by the ResCacheManager, which then starts the
	initialization of the resource.
=====================================================================*/
class AsyncLoadDoneEvent : public Event {
	public:
		///// VARIABLES /////
		static const string sEventType;

		bool		mSuccess;		// true if decompression successful
		size_t		mSize;			// size of the buffer array
		wstring		mResName;		// the resource path
		wstring		mSourceName;	// the name of the ResourceSource
		BufferPtr	mDataPtr;		// the buffer containing data
		ResPtr		mResource;		// shared_ptr to the Resource object being constructed

		///// FUNCTIONS /////
		const string &	type() const { return sEventType; }
		//void	serialize(ostream &out) const {}
		//void	deserialize(istream &in) {}

		// Constructor / destructor
		explicit AsyncLoadDoneEvent(const wstring &resName, const wstring &sourceName,
									const BufferPtr &bPtr, size_t size, const ResPtr &resPtr,
									bool success = true) :
			Event(),
			mResName(resName), mSourceName(sourceName), mDataPtr(bPtr),
			mSize(size), mResource(resPtr), mSuccess(success)
		{}
		virtual ~AsyncLoadDoneEvent() {}
};

/*=====================================================================
class AsyncInitDoneEvent
=====================================================================*/
class AsyncInitDoneEvent : public Event {
	public:
		///// VARIABLES /////
		static const string sEventType;

		bool		mSuccess;		// true if initialization successful
		size_t		mSize;			// size of the buffer array
		wstring		mResName;		// the file loaded from the source object
		wstring		mSourceName;	// the name of the ResourceSource
		BufferPtr	mDataPtr;		// the buffer containing data
		ResPtr		mResource;		// shared_ptr to the Resource object being constructed

		///// FUNCTIONS /////
		const string &	type() const { return sEventType; }

		// Constructor / destructor
		explicit AsyncInitDoneEvent(const wstring &resName, const wstring &sourceName,
									const BufferPtr &bPtr, size_t size, const ResPtr &resPtr,
									bool success = true) :
			Event(),
			mResName(resName), mSourceName(sourceName), mDataPtr(bPtr),
			mSize(size), mResource(resPtr), mSuccess(success)
		{}
		virtual ~AsyncInitDoneEvent() {}
};

/*=============================================================================
class AsyncLoadProcess
=============================================================================*/
class AsyncLoadProcess : public ThreadProcess {
	private:
		///// DEFINITIONS /////
		typedef	hash_map<wstring, size_t>	ThreadIndexMap;

		///// STRUCTURES /////
		/*=====================================================================
		class AsyncLoadListener
		=====================================================================*/
		class AsyncLoadListener : public EventListener {
			friend class AsyncLoadProcess;
			private:
				IEventHandlerPtr	mAsyncLoadHandler;	// we need to hang on to a shared_ptr to the
														// handler for access to its stored queue
			public:
				explicit AsyncLoadListener();
		};

		///// VARIABLES /////
		AsyncLoadListener	mEventListener;
		ThreadIndexMap		mSourceThreadIndexMap;	// for each ResSource, the threadIndex assigned to this thread
		EventManagerPtr		m_eventMgr;

		///// FUNCTIONS /////
		void onUpdate(double deltaMillis) {}

		void threadProc();

	public:
		explicit AsyncLoadProcess(const string &name, const EventManagerPtr &eventMgr);
		~AsyncLoadProcess();
};

/*=============================================================================
class AsyncInitProcess
	This process listens for AsyncLoadDoneEvent and initializes the resource.
	When finished, it puts the AsyncLoadDoneEvent into the staging queue.
=============================================================================*/
class AsyncInitProcess : public ThreadProcess {
	private:
		///// STRUCTURES /////
		/*=====================================================================
		class AsyncInitListener
		=====================================================================*/
		class AsyncInitListener : public EventListener {
			friend class AsyncInitProcess;
			private:
				IEventHandlerPtr	mAsyncLoadDoneHandler;	// we need to hang on to a shared_ptr to the
															// handler for access to its stored queue
			public:
				explicit AsyncInitListener();
		};

		///// VARIABLES /////
		AsyncInitListener	mEventListener;
		EventManagerPtr		m_eventMgr;

		///// FUNCTIONS /////
		void onUpdate(double deltaMillis) {}

		void threadProc();

	public:
		static const string sAsyncLoadShutdownEvent;

		explicit AsyncInitProcess(const string &name, const EventManagerPtr &eventMgr);
		~AsyncInitProcess();
};
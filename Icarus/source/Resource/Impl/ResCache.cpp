/* ResCache.cpp
Author:		Jeff Kiah
Orig.Date:	06/01/2012
*/

#include "Resource/ResCache.h"
#include "Process/ProcessManager.h"
#include "Resource/ResourceProcess.h"
#include "Event/EventManager.h"
#include "Event/RegisteredEvents.h"

///// STRUCTURES /////

/*=====================================================================
class AsyncLoadListener
=====================================================================*/
class ResCacheManager::AsyncLoadListener : public EventListener {
	friend class ResCacheManager;
	private:
		ResCacheManager &mResMgr;
		bool handleInitDone(const EventPtr &ePtr);
	public:
		explicit AsyncLoadListener(ResCacheManager &resMgr);
};

///// VARIABLES /////

// class static vars
const string ResCacheManager::sAsyncLoadShutdownEvent("SYS_RES_ASYNCLOAD_SHUTDOWN");

///// FUNCTIONS /////

// class ResCache

/*---------------------------------------------------------------------
	Calls freeOneResource until new request can fit, returns false when
	request is too large for cache
---------------------------------------------------------------------*/
bool ResCache::makeRoom(size_t sizeB)
{
	if (hasRoom(sizeB)) {
		return true;
	} else if (mLRU.empty()) {
		debugWPrintf(L"ResCache::makeRoom: no resources to release, size=%lu, max cache size=%lu\n", sizeB, mMaxSizeB);
		// when mAllowOversizedResources is true, we indicate true so the one oversized
		// resource will load, but this will be the only one allowed in the cache
		return mAllowOversizedResources;
	}

	// start from the oldest used and work our way back
	auto rend = mLRU.rend();
	for (auto i = mLRU.rbegin(); i != rend; ++i) {
		// if only 2 references, then the mLRU and mResMap contain them so it can be safely removed
		if (i->use_count() == 2) {
			mResMap.erase((*i)->name());			// erase from the hash map
			auto tempIter = mLRU.erase(--i.base());	// erase from the LRU list
			
			if (hasRoom(sizeB)) { return true; }	// if enough room freed for the new resource, we're good

			i = ResCache::ResList::reverse_iterator(tempIter); // using the iterator returned by erase, update the reverse_iter
		}
		// we don't want to remove resources from the cache that still have external references because if
		// a new request comes in for the resource, it will stream a new copy from disk and we'll have
		// duplicates in memory
	}

	debugWPrintf(L"ResCache::makeRoom: out of memory, size=%lu, max cache size %lu\n", sizeB, mMaxSizeB);

	return false; // tried to free resources and still no room
}

/*---------------------------------------------------------------------
	Deletes one resource, the least recently used. Returns false if
	cache is already empty.
---------------------------------------------------------------------*/
/*bool ResCache::freeOneResource()
{
	if (mLRU.empty()) return false;
	
	auto rend = mLRU.rend();
	for (auto i = mLRU.rbegin(); i != rend; ++i) {
		// start from the least recently used and work our way back
		// if only 2 references, the only the mLRU and mResMap contain them so it can be safely removed
		if (i->use_count() == 2) {
			mResMap.erase((*i)->name());
			mLRU.erase(--i.base());
			return true;
		}
		// we don't want to remove resources from the cache that still have external references because if
		// a new request comes in for the resource, it will stream a new copy from disk and we'll have
		// duplicates in memory
	}
	debugPrintf("ResCache::freeOneResource: Unable to free a resource, all resources in use!\n");
	return false;	// we've gone through the whole cache and not one resource can safely be removed
					// if this ever happens, definitely have a problem, need to scale back detail or
					// we are forgetting to release old or unused resources in other places

	// old way
	//ResPtr gonner(mLRU.back());
	//mLRU.pop_back();
	//mResMap.erase(gonner->name());
	//return true;
}*/

/*---------------------------------------------------------------------
	Pushes a resource to the front of the LRU list.
---------------------------------------------------------------------*/
void ResCache::makeMostRecent(const ResMap::iterator &mi)
{
	ResPtr resPtr(*mi->second);	// temporarily store the erased handle
	mLRU.erase(mi->second);		// erase the element from its position in the list
	mLRU.push_front(resPtr);	// make it the most recently used
	mi->second = mLRU.begin();	// fix the iterator stored in the map
}

/*---------------------------------------------------------------------
	If the resource indexed by [key] is present in the cache, return
	true and point resPtr to the resource. Returns false if resource
	not present.
---------------------------------------------------------------------*/
bool ResCache::getResource(ResPtr &resPtr, const wstring &key)
{
	ResMap::iterator mi = mResMap.find(key);
	if (mi == mResMap.end()) { return false; }

	// resource loaded in the cache
	makeMostRecent(mi);		// and make it the most recent
	resPtr = (*mi->second); // return the ResPtr
	return true;
}

/*---------------------------------------------------------------------
	adds a resource to the cache
---------------------------------------------------------------------*/
bool ResCache::addToCache(size_t sizeB, const ResHandle &h)
{
	_ASSERTE(h.isLoaded() && "Trying to add an empty ResPtr to the cache");
	_ASSERTE(!h.name().empty() && "Can't add a Resource to the cache with an empty name");

	// try to find the name in the cache, if it already exists, return false
	if (mResMap.find(h.name()) != mResMap.end()) {
		debugWPrintf(L"ResCache: \"%s\" already exists, add to cache failed!\n", h.name().c_str());
		return false;
	}
	// make sure there is room in the cache
	if (makeRoom(sizeB)) {
		mLRU.push_front(h.mResPtr);			// add the resource to the front of the list
		mResMap[h.name()] = mLRU.begin();	// and insert the iterator into the map
		mUsedB += sizeB;					// and allocate the size in the cache
		h.mResPtr->setResCache(shared_from_this());	// set the resource's weak_ptr to this cache
		return true;
	}
	return false;
}

/*---------------------------------------------------------------------
	adds a resource to cache, name and sizeB taken from the Resource
---------------------------------------------------------------------*/
bool ResCache::addToCache(const ResPtr &resPtr)
{
	const wstring &resName = resPtr->name();
	size_t sizeB = resPtr->sizeB();

	_ASSERTE(resPtr.get() != 0 && "Can't to add an empty ResPtr to the cache");
	_ASSERTE(!resName.empty() && "Can't add a Resource to the cache with an empty name");

	// try to find the name in the cache, if it already exists, return false
	if (mResMap.find(resName) != mResMap.end()) {
		debugWPrintf(L"ResCache: \"%s\" already exists, add to cache failed!\n", resName.c_str());
		return false;
	}
	// make sure there is room in the cache
	if (makeRoom(sizeB)) {
		mLRU.push_front(resPtr);			// add the resource to the front of the list
		mResMap[resName] = mLRU.begin();	// and insert the iterator into the map
		mUsedB += sizeB;					// and allocate the size in the cache
		resPtr->setResCache(shared_from_this());	// set the resource's weak_ptr to this cache
		return true;
	}
	// when mAllowOversizedResources is true, we indicate that the resource has been
	// added so loading succeeds, but it hasn't actually been added
	return mAllowOversizedResources;
}

/*---------------------------------------------------------------------
	forces a specific resource to be removed from the cache
---------------------------------------------------------------------*/
bool ResCache::removeResource(const wstring &key)
{
	ResMap::iterator mi = mResMap.find(key);
	if (mi != mResMap.end()) {
		mLRU.erase(mi->second);	// erase from the LRU list
		mResMap.erase(mi);		// erase from the hash map
		debugWPrintf(L"ResCache: \"%s\" removed from cache: %lu remaining\n", key.c_str(), mLRU.size());
		return true;
	}
	return false;
}

// Constructor / destructor
ResCache::ResCache(size_t sizeMB, bool allowOversizedResources) :
	mMaxSizeB(sizeMB*1024*1024), mUsedB(0), mAllowOversizedResources(allowOversizedResources)
{}

ResCache::~ResCache()
{
	clearCache();
}

// class ResCacheManager

/*---------------------------------------------------------------------
	creates the cache of a certain type passing in the budget, only one
	cache of each type allowed
---------------------------------------------------------------------*/
void ResCacheManager::createCache(ResCacheType cacheType, size_t maxSizeMB, bool allowOversizedResources)
{
	if (mCacheList[cacheType].get() != 0) {
		debugWPrintf(L"ResCacheManager: cache %i already created\n", (int)cacheType);
		return;
	}
	mCacheList[cacheType] = ResCache::create(maxSizeMB, allowOversizedResources);
}

/*---------------------------------------------------------------------
	Pushes an AsyncLoadDoneEvent into the staging list to be picked up
	by tryLoad(). Also removes the entry from the request queue.
---------------------------------------------------------------------*/
void ResCacheManager::addToStagingList(const wstring &resName, const wstring &source, const EventPtr &ePtr)
{
	std::wstringstream ss;
	ss << source << '/' << resName;
	mStagingList[ss.str()] = ePtr;
	mRequestList.erase(ss.str());	// remove entry from the request queue
	debugWPrintf(L"ResCacheManager: \"%s\" added to staging\n", ss.str().c_str());
}

/*---------------------------------------------------------------------
	This will just attempt to pull a resource from a specific cache. If
	the resource does not exist, false is returned and h.mResPtr will
	be empty. h.mSource is ignored with this method, as it bypasses
	the mapping of ResourceSource name to cache type.
---------------------------------------------------------------------*/
bool ResCacheManager::getFromCache(ResHandle &h, ResCacheType cacheType)
{
	_ASSERTE(cacheType < ResCache_MAX && "Bad cacheType");
	// try to find the resource in cache
	ResCachePtr &cache = mCacheList[cacheType];
	if (!cache->getResource(h.mResPtr, h.name())) {
		// not in cache, so return false
		debugWPrintf(L"ResCacheManager: getFromCache(\"%s\", %u) failed, not in cache!\n", h.name().c_str(), cacheType);
		return false;
	}
	return true; // found in the cache
}

/*---------------------------------------------------------------------
	load a new IResourceSource into the system, it should already be
	initialized for use (open() has already been called)
---------------------------------------------------------------------*/
bool ResCacheManager::registerSource(const wstring &srcName, const ResSourcePtr &srcPtr)
{
	ResSourceMap::const_iterator mi = mSourceMap.find(srcName);
	if (mi == mSourceMap.end()) { // source not already registered
		mSourceMap[srcName] = srcPtr;
		debugWPrintf(L"ResCacheManager: source \"%s\" registered\n", srcName.c_str());
		return true;
	} else {
		debugWPrintf(L"ResCacheManager: source \"%s\" already registered!\n", srcName.c_str());
		return false;
	}
}

/*---------------------------------------------------------------------
	Constructs a new object and passes back a shared_ptr. This enforces
	the use of RAII when constructing. Needed because a weak_ptr is
	injected to ResHandle and Resource
---------------------------------------------------------------------*/
ResCacheManagerPtr ResCacheManager::create(size_t availableSysMemMB, size_t availableVidMemMB,
										   const EventManagerPtr &eventMgr, const SchedulerPtr &scheduler)
{
	// create the instance
	ResCacheManagerPtr rcmPtr(new ResCacheManager(availableSysMemMB, availableVidMemMB, eventMgr, scheduler));
	
	// create the thread processes that will process async loading requests

	// register the event to cause threads to exit
	eventMgr->registerEventType(sAsyncLoadShutdownEvent,
								RegEventPtr(new CodeOnlyEvent(EventDataType_Empty)));

	// the load thread is mainly responsible for streaming files from disk (or network I suppose)
	rcmPtr->mLoadThread.reset(new AsyncLoadProcess("AsyncLoadProcess", eventMgr));
	scheduler->attach(rcmPtr->mLoadThread);
	
	// the init thread is responsible for doing initialization in a separate thread (not the main thread)
	// this runs after the load thread has finished streaming the file fully, and before onLoad is run for
	// the new object on the main thread, this way initialization can be totally on the init thread, totally
	// on the main thread, or split up between the two
	rcmPtr->mInitThread.reset(new AsyncInitProcess("AsyncInitProcess", eventMgr));
	scheduler->attach(rcmPtr->mInitThread);

	// send pointer to ResHandle and Resource
	ResHandle::sResCacheManager = rcmPtr;
	Resource::sResCacheManager = rcmPtr;

	return rcmPtr;
}

// Constructor / destructor
ResCacheManager::ResCacheManager(size_t availableSysMemMB, size_t availableVidMemMB,
								 const EventManagerPtr &eventMgr, const SchedulerPtr &scheduler) :
	m_eventMgr(eventMgr),
	m_scheduler(scheduler)
{
	// reserve space for the caches
	mCacheList.reserve(ResCache_MAX);
	for (int c = 0; c < ResCache_MAX; ++c) {
		mCacheList.push_back(ResCachePtr((ResCache*)0));
	}

	// ** NOTE ** these three lines have been moved to Application.cpp to allow
	// for app.config to vary the allocations
	// the following percentages should be data driven not hard coded
	// also should incorporate vid mem into total sizes
	createCache(ResCache_Texture,	(size_t)(availableSysMemMB * 0.35f));
	createCache(ResCache_Material,	(size_t)(availableSysMemMB * 0.05f));
	createCache(ResCache_Geometry,	(size_t)(availableSysMemMB * 0.30f));
	createCache(ResCache_Script,	(size_t)(availableSysMemMB * 0.20f));
	// leaving a 10% buffer
	createCache(ResCache_OnDemand,		0); // zero size means anything can load, but will never be cached
	createCache(ResCache_KeepLoaded,	availableSysMemMB); // large size means always keep resources cached

	m_listener = AsyncLoadListenerUniquePtr(new AsyncLoadListener(*this));
}

ResCacheManager::~ResCacheManager()
{
	// send shutdown event to wake up the threads and allow them to exit incase they are sitting idle
	m_eventMgr->trigger(ResCacheManager::sAsyncLoadShutdownEvent);
}


// class AsyncLoadDoneListener

bool ResCacheManager::AsyncLoadListener::handleInitDone(const EventPtr &ePtr)
{
	// the thread loading/initialization is done, copy the event to a staging
	// area where it will be picked up and put into cache the next time tryLoad
	// is run requesting the resource
	AsyncInitDoneEvent &e = *(static_cast<AsyncInitDoneEvent*>(ePtr.get()));
	mResMgr.addToStagingList(e.mResName, e.mSourceName, ePtr);
	return false; // allow event to propagate
}

ResCacheManager::AsyncLoadListener::AsyncLoadListener(ResCacheManager &resMgr) :
	mResMgr(resMgr),
	EventListener("AsyncLoadListener")
{
	IEventHandlerPtr p(new EventHandler<AsyncLoadListener>(this, &AsyncLoadListener::handleInitDone));
	registerEventHandler(AsyncInitDoneEvent::sEventType, p, 1);
}

/* ResCache.h
Author: Jeff Kiah
Orig.Date: 05/30/2012
*/
#pragma once

#include "Resource/ResCache.h"
#include "Resource/ResourceProcess.h"
#include "Event/EventManager.h"
#include "Utility/Debug.h"

// class ResCache
inline ResCachePtr ResCache::create(size_t sizeMB, bool allowOversizedResources)
{
	return ResCachePtr(new ResCache(sizeMB, allowOversizedResources));
}

/*---------------------------------------------------------------------
	Called when a resource is destroyed, reducing cache total allocated
	This is only called from the IResource destructor and not from any
	function of ResCache directly.
---------------------------------------------------------------------*/
inline void ResCache::memoryHasBeenFreed(size_t sizeB)
{
	mUsedB -= ((sizeB > mUsedB) ? mUsedB : sizeB);
	debugPrintf("ResCache: memory freed, %u bytes\n", sizeB);
}

// class ResCacheManager

/*---------------------------------------------------------------------
	returns a shared_ptr to the ResCache of a given type
---------------------------------------------------------------------*/
inline const ResCachePtr & ResCacheManager::getResCache(ResCacheType cacheType) const
{
	_ASSERTE(cacheType < ResCache_MAX && "Bad cacheType");
	return mCacheList[cacheType];
}

///// TEMPLATE FUNCTIONS /////

/*---------------------------------------------------------------------
	Fetch a resource from cache or a ResSource (disk), ResPtr passed in
	will hold resource if true is returned.
	** NOTE **
	The template param TResource should be a type derived from class
	Resource and MUST implement a constructor with the signature:
		(const string &name, uint sizeB, ResCache *pResCache)
---------------------------------------------------------------------*/
template <typename TResource>
bool ResCacheManager::load(ResHandle &h)
{
	_ASSERTE(TResource::sCacheType < ResCache_MAX && "Bad cacheType");

	// try to find the resource in cache
	ResCachePtr &cache = mCacheList[TResource::sCacheType];
	if (!cache->getResource(h.mResPtr, h.name())) {
		// not in cache, so load it from source and put into cache
		ResSourceMap::const_iterator mi = mSourceMap.find(h.source());
		if (mi != mSourceMap.end()) {
			// loads the resource data from source, returning size or 0 on error
			CharBufferPtr dataPtr((char *)0);
			size_t size = mi->second->getResource(h.name(), dataPtr);
			if (size) {
				// construct a new Resource object, and pass into the ResHandle's ResPtr
				h.mResPtr.reset(new TResource(h.name(), size, cache));

				// store the resource in a cache (specified by the resource)
				bool added = cache->addToCache(size, h);
				if (added) {
					// call the resource's onLoad method
					TResource *pRes = static_cast<TResource*>(h.mResPtr.get());
					pRes->onLoad(dataPtr, false);
					return true;
				} // if not added, cache has no room
			}
		}
		return false;
	}
	return true; // found in the cache
}

/*---------------------------------------------------------------------
	Same as load method, but if resource does not exist in cache, this
	will return immediately and request the resource to be loaded in
	a separate thread, so this is a non blocking call. This method
	supports asynchronous resource loading. The client should call this
	function periodically until it returns success, and then take
	action with the resource. If error is returned the client should
	not expect the resource to load and should stop asking for it.
---------------------------------------------------------------------*/
template <typename TResource>
ResLoadResult ResCacheManager::tryLoad(ResHandle &h)
{
	_ASSERTE(TResource::sCacheType < ResCache_MAX && "Bad cacheType");

	// try to find the resource in cache
	ResCachePtr &cache = mCacheList[TResource::sCacheType];
	if (!cache->getResource(h.mResPtr, h.name())) {
		// not in cache, check staging list to see if raw data has been loaded
		std::wstringstream ss;
		ss << h.source() << '/' << h.name();
		EventQueue::iterator si = mStagingList.find(ss.str());
		if (si != mStagingList.end()) {
			// the raw data is loaded and init thread has run (optionally) but still need to
			// check if loading / init was successful or not
			AsyncInitDoneEvent &e = *(static_cast<AsyncInitDoneEvent*>(si->second.get()));
			if (!e.mSuccess) { return ResLoadResult_Error; }	// error while loading

			// assign the constructed ResPtr into the ResHandle
			h.mResPtr = e.mResource;

			// store the resource in a cache (specified by the resource)
			bool added = cache->addToCache(e.mSize, h);
			if (added) {
				// call the resource's onLoad method
				TResource *pRes = static_cast<TResource*>(h.mResPtr.get());
				pRes->onLoad(e.mDataPtr, true);
			}
			// remove event from staging list
			mStagingList.erase(si);

			// return success if added to cache
			return (added ? ResLoadResult_Success : ResLoadResult_Error);

		} else {
			// data not in staging area, check loading queue to see if it has already been requested
			RequestQueue::const_iterator ri = mRequestList.find(ss.str());
			if (ri == mRequestList.end()) {
				// not yet requested, queue it up to load asynchronously in a thread process
				ResSourceMap::const_iterator mi = mSourceMap.find(h.source());
				if (mi != mSourceMap.end()) {
					// add to request list, index by source/name
					mRequestList.insert(ss.str());

					// construct the resource and pass it into the event
					ResPtr resPtr(new TResource(h.name(), 0, cache)); // size is initially set to 0, must set it during load

					// queues an event for loading thread to pick up
					EventPtr ePtr(new AsyncLoadEvent(h.name(), h.source(), mi->second, resPtr));
					m_eventMgr->raise(ePtr);

				} else {
					return ResLoadResult_Error; // not in the queue, error requesting
				}
			}
		}

		return ResLoadResult_Waiting; // not in the queue, but requested for loading in the background
	}
	return ResLoadResult_Success; // found in the cache
}

/* ResHandle.cpp
Author: Jeff Kiah
Orig.Date: 06/01/2012
*/

#include "Resource/ResHandle.h"

// class ResHandle

ResCacheManagerWeakPtr ResHandle::sResCacheManager;

/*---------------------------------------------------------------------
	This will just attempt to pull a resource from a specific cache. If
	the resource does not exist, false is returned and mResPtr will
	be empty. mSource is always empty with this method, as it bypasses
	the mapping of ResourceSource name to cache type.
---------------------------------------------------------------------*/
bool ResHandle::getFromCache(const wstring &resName, ResCacheType cacheType)
{
	mName = resName;

	// safely grab the ResCacheManager instance
	ResCacheManagerPtr rcm(sResCacheManager.lock());
	if (!rcm) {
		debugWPrintf(L"ResHandle.getFromCache: Error: ResCacheManager pointer is null: \"%s\"\n", resName.c_str());
		return false;
	}

	return rcm->getFromCache(*this, cacheType);
}

// class Resource

ResCacheManagerWeakPtr Resource::sResCacheManager;

/*---------------------------------------------------------------------
	Manually adds the resource to a cache. This would be done if the
	resource is created manually through a derived class constructor,
	and not through the ResHandle creation idiom. This may be
	desireable if a resource needs finer control over construction
	variables that the ResHandle and onLoad() convention don't provide
	for. Returns true on success, false if the resource name already
	exists in the cache.
---------------------------------------------------------------------*/
bool Resource::injectIntoCache(const ResPtr &resPtr, ResCacheType cacheType)
{
	if (resPtr.get() == 0) {
		debugPrintf("Resource.injectIntoCache: Error: cannot inject an empty ResPtr to cache\n");
		return false;
	}
	if (resPtr->name().empty()) {
		debugPrintf("Resource.injectIntoCache: Error: cannot inject a Resource with no name\n");
		return false;
	}

	// safely grab the ResCacheManager instance
	ResCacheManagerPtr rcm(sResCacheManager.lock());
	if (!rcm) {
		debugPrintf("Resource.injectIntoCache: Error: ResCacheManager pointer is null\n");
		return false;
	}
	// returns true if added, false if no room or name already exists
	return rcm->getResCache(cacheType)->addToCache(resPtr);
}

/*---------------------------------------------------------------------
	Manually removes the resource from the managing cache (bypassing
	the cache LRU process). Use this to ensure ResPtr goes out of scope
	so the resource will actually be destroyed and free up system
	memory. Could use for this for resources that you know will not be
	used again.
---------------------------------------------------------------------*/
void Resource::removeFromCache()
{
	// uses weak_ptr to ResCache incase cache does not exist at time of call
	ResCachePtr r(mResCacheWeakPtr.lock());
	if (r) {
		r->removeResource(name());
	}
}

/*---------------------------------------------------------------------
	Notifies cache that this object was destroyed. Because the object
	is owned by a shared_ptr, this only happens once the cache has
	freed it via freeOneResource, and the game object(s) holding onto
	it have also released it or are destroyed.
---------------------------------------------------------------------*/
Resource::~Resource()
{
	// uses weak_ptr to ResCache incase cache does not exist at time of call
	ResCachePtr r(mResCacheWeakPtr.lock());
	if (r) {
		r->memoryHasBeenFreed(sizeB());
	}
}
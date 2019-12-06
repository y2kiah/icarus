/* ResHandle.inl
Author: Jeff Kiah
Orig.Date: 06/01/2012
*/
#pragma once

#include "Resource/ResCache.h"

// class ResHandle

///// TEMPLATE FUNCTIONS /////

/*---------------------------------------------------------------------
	load is used to retrieve a resource from disk or the cache (if
	available) in a sychronous manner. When this blocking call returns,
	the resource will be available, or the loading process will have
	failed. Returns true on success, false on error.
---------------------------------------------------------------------*/
template <typename TResource>
inline bool ResHandle::load(const wstring &resPath)
{
	size_t i = resPath.find_first_of(L"/\\"); // find the first slash or backslash
	if (i == string::npos) { // if no slash found, cannot find the source so return false
		debugWPrintf(L"ResHandle.load: Error: invalid path in load: \"%s\"\n", resPath.c_str());
		return false;
	}
	mSource = resPath.substr(0, i);
	mName = resPath.substr(i+1);

	// safely grab the ResCacheManager instance
	ResCacheManagerPtr rcm(sResCacheManager.lock());
	if (!rcm) {
		debugWPrintf(L"ResHandle.load: Error: ResCacheManager pointer is null: \"%s\"\n", resPath.c_str());
		return false;
	}

	return rcm->load<TResource>(*this);
}

/*---------------------------------------------------------------------
	tryLoad is used to retrieve a resource from disk or the cache (if
	available) in an asynchronous manner. When this non-blocking call
	returns, if the resource was already in the cache, it will be
	available, and otherwise, a job to load it will be queued for a
	worker thread to do the loading. A process should be created to
	monitor the resource handle for the completion or failure of the
	loading.
---------------------------------------------------------------------*/
template <typename TResource>
inline ResLoadResult ResHandle::tryLoad(const wstring &resPath)
{
	size_t i = resPath.find_first_of(L"/\\"); // find the first slash or backslash
	if (i == string::npos) { // if no slash found, cannot find the source so return false
		debugWPrintf(L"ResHandle.tryLoad: Error: invalid path in load: \"%s\"\n", resPath.c_str());
		return ResLoadResult_Error;
	}
	mSource = resPath.substr(0, i);
	mName = resPath.substr(i+1);

	// safely grab the ResCacheManager instance
	ResCacheManagerPtr rcm(sResCacheManager.lock());
	if (!rcm) {
		debugWPrintf(L"ResHandle.tryLoad: Error: ResCacheManager pointer is null: \"%s\"\n", resPath.c_str());
		return ResLoadResult_Error;
	}

	return rcm->tryLoad<TResource>(*this);
}

// class Resource

/*---------------------------------------------------------------------
	Primarily used for resource injection where the cache pointer is
	not set any other way. This is called from addToCache methods.
---------------------------------------------------------------------*/
inline void Resource::setResCache(const ResCachePtr &resCachePtr)
{
	mResCacheWeakPtr = resCachePtr;
}

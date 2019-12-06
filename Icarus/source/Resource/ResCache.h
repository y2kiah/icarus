/* ResCache.h
Author: Jeff Kiah
Orig.Date: 05/30/2012
*/
#pragma once

#include <string>
#include <hash_map>
#include <hash_set>
#include <list>
#include <vector>
#include <memory>
#include "ResHandle.h"
#include "Event/Event.h"

using std::wstring;
using stdext::hash_map;
using stdext::hash_set;
using std::list;
using std::vector;
using std::shared_ptr;
using std::unique_ptr;

// forward declarations
class ResCache;
class IResourceSource;
class Process;
class ResCacheManager;
class EventManager;
class ProcessManager;
typedef shared_ptr<ResCache>		ResCachePtr;
typedef shared_ptr<IResourceSource>	ResSourcePtr;
typedef shared_ptr<char>			CharBufferPtr; // use checked_array_deleter<char> to ensure delete[] called
typedef shared_ptr<Process>			ProcessPtr;
typedef shared_ptr<EventManager>	EventManagerPtr;
typedef shared_ptr<ProcessManager>	SchedulerPtr;

/*=============================================================================
class IResourceSource
	This class could be an interface to a file, memory mapped file, zip file,
	or even a class with logic to algorithmically create data. Another example
	could be a class that handles render-to-texture requests and buffers them.
=============================================================================*/
class IResourceSource : private boost::noncopyable {
	public:
		/*---------------------------------------------------------------------
			This initializes the source. Should be called right after creation.
		---------------------------------------------------------------------*/
		virtual bool	open() = 0;
		virtual size_t	getResourceSize(const wstring &resName) const = 0;
		
		/*---------------------------------------------------------------------
			This loads the resource from the source, passing it back through
			the dataPtr shared_ptr. Returns size, or 0 on error. The third
			parameter can be used for any purpose the derived class requires,
			or can be ignored or made optional. The intention of it is to make
			the getResource() implementation thread-safe by identifying unique
			threads. It is up to the implementer to use it effectively.
		---------------------------------------------------------------------*/
		virtual size_t	getResource(const wstring &resName, CharBufferPtr &dataPtr, size_t threadIndex = 0) = 0;
		
		/*---------------------------------------------------------------------
			Utilize this method to assign unique id's to threads so the calling
			thread can be identified in calls to getResource(). Before a thread
			attempts to call getResource for the first time, it should get and
			store a thread index from the source, so the calling thread can
			identify itself to the source. The source may, for example, store
			unique file handles per thread or any other need for thread safety.
		---------------------------------------------------------------------*/
		virtual size_t	getNewThreadIndex() = 0;

		// Constructor / destructor
		explicit IResourceSource() {}
		virtual ~IResourceSource() {}
};

/*=============================================================================
class ResCache
=============================================================================*/
class ResCache : public std::enable_shared_from_this<ResCache> {
	friend class Resource;	// allows access to call memoryHasBeenFreed() from ~Resource()
	public:
		///// DEFINITIONS /////
		typedef list<ResPtr>	ResList;
		typedef hash_map<wstring, ResList::iterator>	ResMap;

	private:
		///// VARIABLES /////
		ResList	mLRU;		// least recently used list, back is least recent, front is most
		ResMap	mResMap;	// hash map linking the key to the handle

		size_t	mMaxSizeB;	// total memory size in bytes
		size_t	mUsedB;		// total memory allocated in bytes
		
		bool	mAllowOversizedResources; // when false, resources larger than mMaxSizeB will not be loaded

	protected:
		///// FUNCTIONS /////
		/*---------------------------------------------------------------------
			Calls freeOneResource until new request can fit, returns false when
			request is too large for cache
		---------------------------------------------------------------------*/
		bool makeRoom(size_t sizeB);

		/*---------------------------------------------------------------------
			Deletes one resource, the least recently used. Returns false if
			cache is already empty.
		---------------------------------------------------------------------*/
		//bool freeOneResource();

		/*---------------------------------------------------------------------
			Called when a resource is destroyed, reducing cache total allocated
			This is only called from the IResource destructor and not from any
			function of ResCache directly.
		---------------------------------------------------------------------*/
		inline void memoryHasBeenFreed(size_t sizeB);

		/*---------------------------------------------------------------------
			Pushes a resource to the front of the LRU list.
		---------------------------------------------------------------------*/
		void makeMostRecent(const ResMap::iterator &mi);

		/*---------------------------------------------------------------------
			Use create method to construct new cache
		---------------------------------------------------------------------*/
		explicit ResCache(size_t sizeMB, bool allowOversizedResources);

	public:
		/*---------------------------------------------------------------------
			If the resource indexed by {key} is present in the cache, return
			true and point resPtr to the resource. Returns false if resource
			not present.
		---------------------------------------------------------------------*/
		bool getResource(ResPtr &resPtr, const wstring &key);

		/*---------------------------------------------------------------------
			adds a resource to the cache
		---------------------------------------------------------------------*/
		bool addToCache(size_t sizeB, const ResHandle &h);
		
		/*---------------------------------------------------------------------
			adds a resource to cache, name and sizeB taken from the Resource
		---------------------------------------------------------------------*/
		bool addToCache(const ResPtr &resPtr);

		/*---------------------------------------------------------------------
			forces a specific resource to be removed from the cache
		---------------------------------------------------------------------*/
		bool removeResource(const wstring &key);

		/*---------------------------------------------------------------------
			clears the entire resource list
		---------------------------------------------------------------------*/
		void clearCache() { mLRU.clear(); mResMap.clear(); }

		// Accessors
		bool	hasRoom(size_t sizeB) const	{ return (mMaxSizeB - mUsedB >= sizeB); }
		size_t	maxSizeBytes() const		{ return mMaxSizeB; }
		size_t	usedBytes() const			{ return mUsedB; }

		// Constructor / destructor
		inline static ResCachePtr create(size_t sizeMB, bool allowOversizedResources = true);
		~ResCache();
};

/*=============================================================================
class ResCacheManager
=============================================================================*/
class ResCacheManager :
	public std::enable_shared_from_this<ResCacheManager>
{
	friend class AsyncLoadDoneListener;		// provide access to staging list
	public:
		///// DEFINITIONS /////
		typedef hash_map<wstring, ResSourcePtr>	ResSourceMap;
		typedef vector<ResCachePtr>				ResCacheList;
		typedef hash_map<wstring, EventPtr>		EventQueue;
		typedef hash_set<wstring>				RequestQueue;

	private:
		///// STRUCTURES /////
		class AsyncLoadListener;
		typedef unique_ptr<AsyncLoadListener> AsyncLoadListenerUniquePtr;

		///// VARIABLES /////
		ResSourceMap	mSourceMap;		// the table of registered source files
		ResCacheList	mCacheList;		// the list of resource caches, one for each ResCacheType

		// For async threaded loading
		AsyncLoadListenerUniquePtr	m_listener;	// listens for AsyncLoadDone event and pushes event into staging queue
		EventQueue		mStagingList;	// data that has been loaded by another thread but not yet cached
		RequestQueue	mRequestList;	// list of pending resources already requested via tryLoad, makes sure
										// a request isn't submitted multiple times for the same resource
		ProcessPtr		mLoadThread;	// pointer to the loader thread process, so it can be detached in destructor
		ProcessPtr		mInitThread;	// pointer to the init thread process, so it can be detached in destructor

		// Dependencies
		EventManagerPtr	m_eventMgr;
		SchedulerPtr	m_scheduler;

		///// FUNCTIONS /////
		bool inStagingList(const wstring &resName, const string &source);

		/*---------------------------------------------------------------------
			pushes an AsyncLoadDoneEvent into the staging list to be picked up
			by tryLoad()
		---------------------------------------------------------------------*/
		void addToStagingList(const wstring &resName, const wstring &source, const EventPtr &ePtr);

	protected:
		// constructor protected due to enable_shared_from_this, use create() method instead
		explicit ResCacheManager(size_t availableSysMemMB, size_t availableVidMemMB,
								 const EventManagerPtr &eventMgr, const SchedulerPtr &scheduler);

	public:
		/*---------------------------------------------------------------------
			This string defines the event that shuts down the child threads
		---------------------------------------------------------------------*/
		static const string sAsyncLoadShutdownEvent;

		/*---------------------------------------------------------------------
			creates the cache of a certain type passing in the budget, only one
			cache of each type allowed
		---------------------------------------------------------------------*/
		void createCache(ResCacheType cacheType, size_t maxSizeMB, bool allowOversizedResources = true);

		/*---------------------------------------------------------------------
			returns a shared_ptr to the ResCache of a given type
		---------------------------------------------------------------------*/
		inline const ResCachePtr & getResCache(ResCacheType cacheType) const;

		/*---------------------------------------------------------------------
			returns a shared_ptr to the child thread processes
		---------------------------------------------------------------------*/
		const ProcessPtr & getLoadThread() const { return mLoadThread; }
		const ProcessPtr & getInitThread() const { return mInitThread; }

		/*---------------------------------------------------------------------
			Fetch a resource from cache or a ResSource (disk), ResPtr passed in
			will hold resource if true is returned.
			** NOTE **
			The template param TResource should be a type derived from class
			Resource and MUST implement a constructor with the signature:
			(const string &name, uint sizeB, const ResCachePtr &resCachePtr)
		---------------------------------------------------------------------*/
		template <typename TResource>
		bool load(ResHandle &h);

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
		ResLoadResult tryLoad(ResHandle &h);

		/*---------------------------------------------------------------------
			This will just attempt to pull a resource from a specific cache. If
			the resource does not exist, false is returned and h.mResPtr will
			be empty. h.mSource is ignored with this method, as it bypasses
			the mapping of ResourceSource name to cache type.
		---------------------------------------------------------------------*/
		bool getFromCache(ResHandle &h, ResCacheType cacheType);

		/*---------------------------------------------------------------------
			load a new IResourceSource into the system, it should be
			initialized for use externally (open() still needs to be called)
		---------------------------------------------------------------------*/
		bool registerSource(const wstring &srcName, const ResSourcePtr &srcPtr);

		// Constructor / destructor
		static ResCacheManagerPtr create(size_t availableSysMemMB, size_t availableVidMemMB,
										 const EventManagerPtr &eventMgr, const SchedulerPtr &scheduler);

		~ResCacheManager();
};

#include "Impl/ResCache.inl"
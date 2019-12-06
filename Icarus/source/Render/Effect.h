/* Effect.h
Author: Jeff Kiah
Orig.Date: 06/24/2012
*/
#pragma once

#include "Resource/ResHandle.h"

template <class Implementation>
class Effect_Base : public Resource {
	public:
		/*---------------------------------------------------------------------
			In derived classes, should return true if the onThreadInit method
			is to be used for initialization off the main thread.
		---------------------------------------------------------------------*/
		virtual bool useThreadInit() const {
			return static_cast<const Implementation*>(this)->useThreadInit();
		}

		/*---------------------------------------------------------------------
			onLoad is called automatically by the resource caching system when
			a resource is first loaded from disk and added to the cache. This
			function loads pre-compiled shaders only.
		---------------------------------------------------------------------*/
		virtual bool onLoad(const CharBufferPtr &dataPtr, bool async) {
			return static_cast<Implementation*>(this)->onLoad(dataPtr, async);
		}

		/*---------------------------------------------------------------------
			onThreadInit performs initialization in the init thread after the
			resource bytes have been streamed from disk
		---------------------------------------------------------------------*/
		virtual bool onThreadInit(const CharBufferPtr &dataPtr) {
			return static_cast<Implementation*>(this)->onThreadInit(dataPtr);
		}

		virtual ~Effect_Base() {}

		// Static Variables
		// specifies the cache that will manager the resource
		static const ResCacheType	sCacheType = ResCache_Material;

	protected:

		explicit Effect_Base(const wstring &name, size_t sizeB, const ResCachePtr &resCachePtr) :
			Resource(name, sizeB, resCachePtr)
		{}
		explicit Effect_Base() :
			Resource()
		{}
};

#if defined(WIN32)
#include "Impl/Effect_D3D11.h"
#endif
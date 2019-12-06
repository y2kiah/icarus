/* Texture2D.h
Author: Jeff Kiah
Orig.Date: 06/01/2012
*/
#pragma once

#include <cstdint>
#include "Resource/ResHandle.h"

///// STRUCTURES /////

struct Texture2DCreateDesc {
	const void *srcData;		// source data, null for empty texture, Texture2D expects data in RGBA color order
	size_t		size;			// size of source data buffer, 0 if srcData==null
	uint32_t	srcChannels;	// channels (1,2 or 4), Texture2D expects 8 bits per channel (for now)
	uint32_t	srcWidth;		// image width
	uint32_t	srcHeight;		// image height
	uint32_t	hasMipLevels;	// mipmap levels included in source data (>= 1)
	bool		genMipmapChain;	// true to generate full mipmap chain
	bool		compress;		// true to use BC compression when appropriate, ignored and made false if genMipmapChain is true
	//TextureDataFormat	_format;
	//TextureUsageMode	_usage;
	//TextureRepeatMode	_repeat;
	//TextureFilterMode	_filter;
};

template <class Implementation>
class Texture2D_Base : public Resource {
	public:
		///// FUNCTIONS /////
		// Accessors
		bool		initialized() const	{ return mInitialized; }
		uint32_t	width() const		{ return mWidth; }
		uint32_t	height() const		{ return mHeight; }

		// Mutators
		/*---------------------------------------------------------------------
			These virtual methods must be implemented for Resource system.
			No need to implement here in this base class since they will be
			runtime polymorphic anyway.
		---------------------------------------------------------------------*/
		virtual bool useThreadInit() const = 0;
		virtual bool onLoad(const CharBufferPtr &dataPtr, bool async) = 0;
		virtual bool onThreadInit(const CharBufferPtr &dataPtr) = 0;
		//bool onLoad(const CharBufferPtr &dataPtr, bool async) {
		//	return static_cast<Implementation*>(this)->onLoad(dataPtr, async);
		//}
		
		/*---------------------------------------------------------------------
			Loads a texture directly from a file (not through the Resource-
			Source interface. Use this with the cache injection method.
		---------------------------------------------------------------------*/
		bool loadFromFile(const wstring &filename, uint32_t maxSize = 0) {
			return static_cast<Implementation*>(this)->loadFromFile(filename, maxSize);
		}

		/*---------------------------------------------------------------------
			Creates a texture from data in memory (not through the Resource-
			Source interface). Use this with the cache injection method.
		---------------------------------------------------------------------*/
		bool createTexture(Texture2DCreateDesc &desc, const wstring &name) {
			return static_cast<Implementation*>(this)->createTexture(desc, name);
		}

		// Misc Functions
		/*---------------------------------------------------------------------
			Save the in-memory texture to disk.
		---------------------------------------------------------------------*/
		bool saveTextureToDDSFile(const wstring &filename) {
			return static_cast<Implementation*>(this)->saveTextureToDDSFile(filename);
		}

		/*---------------------------------------------------------------------
			releases the texture resource and resets metadata
		---------------------------------------------------------------------*/
		void clearImageData() {
			return static_cast<Implementation*>(this)->clearImageData();
		}

		// Destructor
		virtual ~Texture2D_Base() {}

		///// VARIABLES /////
		// Static Variables
		
		// specifies the cache that will manager the resource
		static const ResCacheType	sCacheType = ResCache_Texture;

	protected:
		// Member Variables
		bool		mInitialized;
		uint32_t	mWidth, mHeight;

		// Protected constructors ensure this class isn't instantiated directly

		/*---------------------------------------------------------------------
			constructor with this signature is required for the resource system
		---------------------------------------------------------------------*/
		explicit Texture2D_Base(const wstring &name, size_t sizeB, const ResCachePtr &resCachePtr) :
			Resource(name, sizeB, resCachePtr),
			mInitialized(false), mWidth(0), mHeight(0)
		{}
		
		/*---------------------------------------------------------------------
			use this default constructor to create the texture without caching,
			or for cache injection method.
		---------------------------------------------------------------------*/
		explicit Texture2D_Base() :
			Resource(),
			mInitialized(false), mWidth(0), mHeight(0)
		{}
};

#if defined(WIN32)
#include "Impl/Texture2D_d3d11.h"
#endif
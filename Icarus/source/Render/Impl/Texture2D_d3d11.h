/* Texture2D_d3d11.h
Author: Jeff Kiah
Orig.Date: 06/01/2012
*/
#pragma once
#if defined(WIN32)

#include "Render/Texture2D.h"
#include "D3D11_Defs.h"
#include "Renderer_d3d11.h"

///// STRUCTURES /////

/*=============================================================================
class Texture2D_D3D11
=============================================================================*/
class Texture2D_D3D11 : public Texture2D_Base<Texture2D_D3D11> {
	friend class Texture2D_Base<Texture2D_D3D11>;
	friend class Renderer_D3D11;

	private:
		///// VARIABLES /////
		static RendererWeakPtr		sRenderer;	// injected from renderer_d3d11 when it's created

		ID3D11Texture2DPtr			mTexture2D;
		ID3D11ShaderResourceViewPtr	mShaderResourceView;
		D3D11_TEXTURE2D_DESC		mDesc;

		///// FUNCTIONS /////
		/*---------------------------------------------------------------------
			Sets an estimated size based on texture size, mip levels, and
			compression. Used when texture is loaded from file.
		---------------------------------------------------------------------*/
		void setSizeEstimate();

		// Accessors
		/*---------------------------------------------------------------------
			returns the device and device context from renderer
		---------------------------------------------------------------------*/
		static ID3D11DevicePtr getD3D11DevicePtr();
		static ID3D11DeviceContextPtr getD3D11DeviceContextPtr();

	public:
		const ID3D11Texture2DPtr &getTexture2D() const { return mTexture2D; }
		const ID3D11ShaderResourceViewPtr &getShaderResourceView() const { return mShaderResourceView; }
		const RendererWeakPtr &getRenderer() const { return sRenderer; }

		/*---------------------------------------------------------------------
			Indicate that onThreadInit method is used for async loading pattern
		---------------------------------------------------------------------*/
		virtual bool useThreadInit() const { return true; }

		// Mutators
		/*---------------------------------------------------------------------
			Loads a texture directly from a file (not through the Resource-
			Source interface). Use this with the cache injection method.
		---------------------------------------------------------------------*/
		bool loadFromFile(const wstring &filename, uint32_t maxSize = 0);

		/*---------------------------------------------------------------------
			Creates a texture from data in memory (not through the Resource-
			Source interface). Uses the DirectXTex library, use for tool dev
			and not for real time.
		---------------------------------------------------------------------*/
		bool createTexture(Texture2DCreateDesc &desc, const wstring &name);

		/*---------------------------------------------------------------------
			onLoad is called automatically by the resource caching system when
			a resource is first loaded from disk and added to the cache. This
			function takes important loading parameters directly from the file
			like width, height, and format. The full mipmap chain will be
			created. For DDS files, the number of levels is taken directly from
			the file, so the only way to ensure mipmaps will not be generated
			is to store the texture as DDS with only 1 level. For finer control
			over the number of levels loaded, use one of the other load
			functions and manually inject into the cache.
		---------------------------------------------------------------------*/
		virtual bool onLoad(const CharBufferPtr &dataPtr, bool async);

		/*---------------------------------------------------------------------
			onThreadInit should perform any initialization that can be done
			from the init thread after the resource bytes have been streamed
			from disk, and before onLoad is run on the main thread. This class
			uses this function for texture initialization on the d3d device.
		---------------------------------------------------------------------*/
		virtual bool onThreadInit(const CharBufferPtr &dataPtr);

		// Misc functions
		/*---------------------------------------------------------------------
			Save the in-memory texture to disk. Uses DirectXTex library, use
			for tool dev and not for real time.
		---------------------------------------------------------------------*/
		#if defined(ICARUS_DEV_TOOLS)
		bool saveTextureToDDSFile(const wstring &filename);
		#endif

		/*---------------------------------------------------------------------
			releases the texture resource and resets metadata
		---------------------------------------------------------------------*/
		void clearImageData();

		// Constructors
		/*---------------------------------------------------------------------
			constructor with this signature is required for the resource system
		---------------------------------------------------------------------*/
		explicit Texture2D_D3D11(const wstring &name, size_t sizeB, const ResCachePtr &resCachePtr) :
			Texture2D_Base(name, sizeB, resCachePtr)
		{}

		/*---------------------------------------------------------------------
			use this default constructor to create the texture without caching,
			or for cache injection method.
		---------------------------------------------------------------------*/
		explicit Texture2D_D3D11() :
			Texture2D_Base()
		{}

		// Destructor
		virtual ~Texture2D_D3D11() {}
};

typedef Texture2D_Base<Texture2D_D3D11> Texture2D;
typedef Texture2D_D3D11					Texture2DImpl;
typedef shared_ptr<Texture2D>			Texture2DPtr;

#endif
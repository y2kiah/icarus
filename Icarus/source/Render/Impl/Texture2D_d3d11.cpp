/* Texture_d3d11.cpp
Author: Jeff Kiah
Orig.Date: 06/01/2012
*/
#include "Texture2D_D3D11.h"

#if defined(WIN32)

#include "Utility/Debug.h"
//#include "Application/Engine.h"
#include "DirectXTex/DDSTextureLoader.h"

///// STATIC VARIABLES /////

RendererWeakPtr Texture2D_D3D11::sRenderer;

///// FUNCTIONS /////

/*---------------------------------------------------------------------
	Loads a texture directly from a file (not through the Resource-
	Source interface). Use this with the cache injection method.
---------------------------------------------------------------------*/
bool Texture2D_D3D11::loadFromFile(const wstring &filename, uint32_t maxSize)
{
	_ASSERTE(maxSize >= 0 && "Texture2D_D3D11::loadFromFile bad input");

	if (initialized()) { return false; }

	ID3D11Texture2D *newTex;
	ID3D11ShaderResourceView *srView;
	// get the D3D11Device
	ID3D11DevicePtr d3dDevice = getD3D11DevicePtr();
	if (!d3dDevice) { return false; }

	// create the texture, assume dds
	HRESULT hr = CreateDDSTextureFromFile(
					d3dDevice,
					filename.c_str(),
					(ID3D11Resource**)&newTex,
					&srView,
					maxSize); // maxsize should come from settings
	if (FAILED(hr)) {
		_com_error err(hr);
		debugWPrintf(L"Texture2D_D3D11: Load from file failed \"%s\": %s\n", filename.c_str(), err.ErrorMessage());
		return false;
	}
	
	mTexture2D.Attach(newTex, false);
	mShaderResourceView.Attach(srView, false);

	// get actuals of created texture
	mName = filename;
	mTexture2D->GetDesc(&mDesc);
	mWidth = mDesc.Width;
	mHeight = mDesc.Height;
	setSizeEstimate();
	mInitialized = true;

	debugWPrintf(L"Texture \"%s\" loaded: %ux%u, levels=%u, format=%u, sizeKB=%u\n", mName.c_str(), mWidth, mHeight,
				mDesc.MipLevels, mDesc.Format, mSizeB/1024);

	return true;
}

/*---------------------------------------------------------------------
	Creates a texture from data in memory (not through the Resource-
	Source interface). Uses the DirectXTex library, use for tool dev
	and not for real time.
---------------------------------------------------------------------*/
bool Texture2D_D3D11::createTexture(Texture2DCreateDesc &desc, const wstring &name)
{
	_ASSERTE(desc.srcChannels >= 1 && desc.srcChannels <= 4 && desc.hasMipLevels > 0);
//	_ASSERTE(_usage == TEX_USAGE_DEFAULT || _usage == TEX_USAGE_DYNAMIC || _usage == TEX_USAGE_DISPLACEMENT_MAP);

	if (initialized()) { return false; }

	// fix input data
	if (!desc.srcData) { desc.size = 0; }
	if (desc.genMipmapChain) { desc.compress = false; }
	if (!desc.size) { desc.genMipmapChain = false; }

	// get the D3D11Device and context
	ID3D11DevicePtr d3dDevice = getD3D11DevicePtr();
	if (!d3dDevice) { return false; }
	ID3D11DeviceContextPtr d3dContext = getD3D11DeviceContextPtr();
	if (!d3dContext) { return false; }

	// get the format
	DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM;
	if (desc.compress) {
		format = DXGI_FORMAT_BC7_UNORM;

	} else {
		switch (desc.srcChannels) {
			case 1:	format = DXGI_FORMAT_R8_UNORM; break;
			case 2:	format = DXGI_FORMAT_R8G8_UNORM; break;
			// note for 3 channels, hardware R8G8B8A8 is used
		}
	}

	// put together the description
	mDesc.Width = desc.srcWidth;
	mDesc.Height = desc.srcHeight;
	mDesc.MipLevels = (desc.genMipmapChain ? 0 : desc.hasMipLevels);
	mDesc.ArraySize = 1;
	mDesc.Format = format;
	mDesc.SampleDesc.Count = 1;
	mDesc.SampleDesc.Quality = 0;
	mDesc.Usage = D3D11_USAGE_DEFAULT;
	mDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | (desc.genMipmapChain ? D3D11_BIND_RENDER_TARGET : 0); // needs to be render target to generate mipmap sublevels
	mDesc.CPUAccessFlags = 0;
	mDesc.MiscFlags = (desc.genMipmapChain ? D3D11_RESOURCE_MISC_GENERATE_MIPS : 0);

	// put together the data array
	unique_ptr<D3D11_SUBRESOURCE_DATA[]> data;
	
	// write pointers to initialization data
	if (desc.srcData) {
		data.reset(new D3D11_SUBRESOURCE_DATA[desc.hasMipLevels]);
		uint8_t *ptr = (uint8_t*)desc.srcData;
		for (uint32_t m = 0; m < desc.hasMipLevels; ++m) {
			data[m].pSysMem = ptr;
			data[m].SysMemPitch = desc.srcWidth * desc.srcChannels;
			data[m].SysMemSlicePitch = ((desc.srcChannels * desc.srcWidth * desc.srcHeight) >> (m << 1));
			ptr += data[m].SysMemSlicePitch;
		}
	}

	// create the texture
	ID3D11Texture2D *newTex = 0;
	HRESULT hr = d3dDevice->CreateTexture2D(&mDesc, data.get(), &newTex);
	if (FAILED(hr)) {
		return false;
	}
	mTexture2D.Attach(newTex, false);

	// create shader resource view
	ID3D11ShaderResourceView *srView = 0;
	hr = d3dDevice->CreateShaderResourceView(newTex, 0, &srView);
	if (FAILED(hr)) {
		return false;
	}
	mShaderResourceView.Attach(srView, false);

	if (desc.genMipmapChain) {
		d3dContext->GenerateMips(srView);
	}
	
	// set member variables
	mName = name;
	mWidth = desc.srcWidth;
	mHeight = desc.srcHeight;
	mInitialized = true;

	return true;
}

#if defined(ICARUS_DEV_TOOLS)

#if defined(_DEBUG)
#pragma comment ( lib, "d3dx11d.lib" )
#else
#pragma comment ( lib, "d3dx11.lib" )
#endif
// using d3dx11 temporarily for saveToDDSFile until I can get DirectXTex to work
#include <d3dx11.h>
//#include "DirectXTex/DirectXTex.h"

/*---------------------------------------------------------------------
	Save the in-memory texture to disk. Uses DirectXTex library, use
	for tool dev and not for real time.
---------------------------------------------------------------------*/
bool Texture2D_D3D11::saveTextureToDDSFile(const wstring &filename)
{
	if (!initialized()) {
		debugWPrintf(L"Texture2D_D3D11: cannot save uninitialized texture \"%s\"\n", filename.c_str());
		return false;
	}

	// get the D3D11Device and context
	ID3D11DevicePtr d3dDevice = getD3D11DevicePtr();
	if (!d3dDevice) { return false; }
	ID3D11DeviceContextPtr d3dContext = getD3D11DeviceContextPtr();
	if (!d3dContext) { return false; }

	// this uses D3DX11 implementation which is deprecated. Below is an attemp to use DirectXTex but LoadFromDDSMemory is throwing an error
	HRESULT hr = D3DX11SaveTextureToFile(d3dContext, mTexture2D, D3DX11_IFF_DDS, filename.c_str());
	if (FAILED(hr)) {
		_com_error err(hr);
		debugWPrintf(L"Texture2D_D3D11: Save to file failed \"%s\": %s\n", filename.c_str(), err.ErrorMessage());
		return false;
	}

/*
	// create a new texture with CPU_ACCESS_READ flag and use CopyResource to copy the texture on the GPU
	D3D11_TEXTURE2D_DESC desc(mDesc);
	desc.Usage = D3D11_USAGE_STAGING; // A resource that supports data transfer (copy) from the GPU to the CPU
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	desc.BindFlags = 0;

	ID3D11Texture2D *pTemp = 0;
	ID3D11Texture2DPtr tempPtr;
	HRESULT hr = d3dDevice->CreateTexture2D(&desc, 0, &pTemp);
	if (FAILED(hr)) {
		debugWPrintf(L"Texture2D_D3D11: CreateTexture2D failed \"%s\"\n", filename.c_str());
		return false;
	}
	tempPtr.Attach(pTemp, false);

	d3dContext->CopyResource(pTemp, mTexture2D);
	
	// Read back from GPU to CPU
	DirectX::ScratchImage img;
	DirectX::TexMetadata imgMetaData;
	// map pointer to texture data, CPU can read, GPU denied access
	D3D11_MAPPED_SUBRESOURCE msr;
	hr = d3dContext->Map(pTemp, D3D11CalcSubresource(0,0,1), D3D11_MAP_READ, 0, &msr);
	if (FAILED(hr)) {
		debugWPrintf(L"Texture2D_D3D11: failed to Map resource \"%s\"\n", filename.c_str());
		return false;
	}

	hr = DirectX::LoadFromDDSMemory((const void *)msr.pData, sizeof(msr.pData),
									DirectX::DDS_FLAGS_NONE, &imgMetaData, img);

	d3dContext->Unmap(pTemp, D3D11CalcSubresource(0,0,1)); // invalidate CPU-readable pointer, give texture back to GPU

	if (FAILED(hr)) {
		_com_error err(hr);
		debugWPrintf(L"Texture2D_D3D11: LoadFromDDSMemory failed \"%s\": %s\n", filename.c_str(), err.ErrorMessage());
		return false;
	}

	// save the image to disk
	hr = DirectX::SaveToDDSFile(*img.GetImage(0,0,0), DirectX::DDS_FLAGS_NONE, filename.c_str());
	if (FAILED(hr)) {
		_com_error err(hr);
		debugWPrintf(L"Texture2D_D3D11: SaveToDDSFile failed \"%s\": %s\n", filename.c_str(), err.ErrorMessage());
		return false;
	}*/
	
	return true;
}
#endif

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
bool Texture2D_D3D11::onLoad(const CharBufferPtr &dataPtr, bool async)
{
	// if the resource is loaded using async method, onThreadInit will
	// be called automatically on a separate thread. If loading sync styley,
	// we need to call the routine to perform the same actions
	if (!async) {
		return onThreadInit(dataPtr);
	}
	return true;
}

/*---------------------------------------------------------------------
	onThreadInit should perform any initialization that can be done
	from the init thread after the resource bytes have been streamed
	from disk, and before onLoad is run on the main thread. This class
	uses this function for texture initialization on the d3d device.
---------------------------------------------------------------------*/
bool Texture2D_D3D11::onThreadInit(const CharBufferPtr &dataPtr)
{
	ID3D11Texture2D *newTex;
	ID3D11ShaderResourceView *srView;
	// get the D3D11Device
	ID3D11DevicePtr d3dDevice = getD3D11DevicePtr();
	if (!d3dDevice) { return false; }

	// create the texture, assume dds
	HRESULT hr = CreateDDSTextureFromMemory(
					d3dDevice,
					(const uint8_t *)dataPtr.get(),
					sizeB(),
					(ID3D11Resource**)&newTex,
					&srView,
					0); // maxsize should come from settings
	if (FAILED(hr)) {
		_com_error err(hr);
		debugWPrintf(L"Texture2D_D3D11: CreateDDSTextureFromMemory failed \"%s\": %s\n", mName.c_str(), err.ErrorMessage());
		return false;
	}
	
	mTexture2D.Attach(newTex, false);
	mShaderResourceView.Attach(srView, false);

	// get actuals of created texture
	mTexture2D->GetDesc(&mDesc);
	mWidth = mDesc.Width;
	mHeight = mDesc.Height;
	mInitialized = true;

	debugWPrintf(L"Texture \"%s\" loaded: %ux%u, levels=%u, format=%u, sizeKB=%u\n", mName.c_str(), mWidth, mHeight,
				mDesc.MipLevels, mDesc.Format, mSizeB/1024);

	return true;
}

/*---------------------------------------------------------------------
	Sets an estimated size based on texture size, mip levels, and
	compression. Used when texture is loaded from file.
---------------------------------------------------------------------*/
void Texture2D_D3D11::setSizeEstimate()
{
	mSizeB = 4 * mWidth * mHeight;	// rough estimate assumes 4Bpp ARGB
	size_t levelSize = mSizeB;
	for (uint32_t l = 1; l < mDesc.MipLevels; ++l) {
		levelSize >>= 2;
		mSizeB += levelSize;
	}
	// if using DXT compression, reduce the size
	switch (mDesc.Format) {
		case DXGI_FORMAT_BC1_UNORM: case DXGI_FORMAT_BC4_UNORM:
			mSizeB /= 8; break;	// these formats are 8 bytes per 4x4 pixel block
		case DXGI_FORMAT_BC2_UNORM: case DXGI_FORMAT_BC3_UNORM: case DXGI_FORMAT_BC5_UNORM:
		case DXGI_FORMAT_BC6H_UF16: case DXGI_FORMAT_BC7_UNORM:
			mSizeB /= 4; break;	// these formats are 16 bytes per 4x4 pixel block
	}
}

/*---------------------------------------------------------------------
	releases the texture resource and resets metadata
---------------------------------------------------------------------*/
void Texture2D_D3D11::clearImageData()
{
	mTexture2D = 0;
	mInitialized = false;
	mWidth = mHeight = 0;
	ZeroMemory(&mDesc, sizeof(mDesc));
}

/*---------------------------------------------------------------------
	returns the device and device context from Engine::instance()
---------------------------------------------------------------------*/
ID3D11DevicePtr Texture2D_D3D11::getD3D11DevicePtr()
{
	// get Renderer from the Engine singleton instance
	RendererImpl *pRend = static_cast<RendererImpl *>(sRenderer.lock().get());
	if (!pRend) {
		_ASSERTE(false && "Attempted to get null Renderer pointer");
		return ID3D11DevicePtr();
	}

	// get the D3D11Device
	if (!pRend->getDevice()) {
		_ASSERTE(false && "Attempted to get null D3D11Device pointer");
		return ID3D11DevicePtr();
	}

	return pRend->getDevice();
}

ID3D11DeviceContextPtr Texture2D_D3D11::getD3D11DeviceContextPtr()
{
	// get Renderer from the Engine singleton instance
	RendererImpl *pRend = static_cast<RendererImpl *>(sRenderer.lock().get());
	if (!pRend) {
		_ASSERTE(false && "Attempted to get null Renderer pointer");
		return ID3D11DevicePtr();
	}

	// get the D3D11DeviceContext
	if (!pRend->getImmediateContext()) {
		_ASSERTE(false && "Attempted to get null D3D11DeviceContext pointer");
		return ID3D11DeviceContextPtr();
	}

	return pRend->getImmediateContext();
}

#endif
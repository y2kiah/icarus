/* Effect_D3D11.cpp
Author: Jeff Kiah
Orig.Date: 06/24/2012
*/

#include "Effect_D3D11.h"

#if defined(WIN32)

#include <cstdio>
#include "Utility/Debug.h"

#if defined(ICARUS_DEV_TOOLS)
#include <d3dcompiler.h>
#endif

// ifx file format is:
//	byte 0-3	= "ifx" + null "ifx\0"
//	byte 4-7	= (uint32_t) version (1)
//	byte 8-11	= (uint32_t) lengthVS (bytes)
//	byte 12-15	= (uint32_t) lengthPS (bytes)
//	byte 16		= VS start
//	byte 16 + lengthVS = PS start
struct IfxHeader {
	char ifx[4];
	uint32_t version;
	uint32_t lengthVS;
	uint32_t lengthPS;
};
const size_t ifxHeaderSize = sizeof(IfxHeader);

// Class static vars
RendererWeakPtr Effect_D3D11::sRenderer;

// Functions
/*---------------------------------------------------------------------
	return true for effect to initialize in init thread
---------------------------------------------------------------------*/
bool Effect_D3D11::useThreadInit() const
{
	return true;
}

/*---------------------------------------------------------------------
	onLoad is called automatically by the resource caching system when
	a resource is first loaded from disk and added to the cache. This
	function loads pre-compiled shaders only. Expect file type .ifx
---------------------------------------------------------------------*/
bool Effect_D3D11::onLoad(const CharBufferPtr &dataPtr, bool async)
{
	// if the resource is loaded using async method, onThreadInit will
	// be called automatically on a separate thread. If loading sync=true,
	// we need to call the routine to perform the same actions
	if (!async) {
		return onThreadInit(dataPtr);
	}
	return true;
}

/*---------------------------------------------------------------------
	onThreadInit performs initialization in the init thread after the
	resource bytes have been streamed from disk
---------------------------------------------------------------------*/
bool Effect_D3D11::onThreadInit(const CharBufferPtr &dataPtr)
{
	ID3D11Device &device =
		static_cast<Renderer_D3D11*>(sRenderer.lock().get())->getDevice();

	// get header info
	char *pData = dataPtr.get();
	IfxHeader hdr = *(IfxHeader*)pData;
	
	// check for valid file header
	if (_strnicmp(hdr.ifx, "ifx", 3) != 0) {
		debugWPrintf(L"Effect_D3D11: onLoad failed: file %s not recognized\n", mName.c_str());
		return false;
	}

	// make sure actual size matches
	if (sizeB() != hdr.lengthVS + hdr.lengthPS + ifxHeaderSize) {
		debugWPrintf(L"Effect_D3D11: onLoad failed: file %s size does not correspond with shader lengths\n", mName.c_str());
		return false;
	}

	// create vertex shader
	pData += ifxHeaderSize; // position the pointer to VS start
	HRESULT hr = device.CreateVertexShader((const void *)pData, hdr.lengthVS,
											NULL, &mpVertexShader.GetInterfacePtr());
	if (FAILED(hr)) {
		_com_error err(hr);
		debugWPrintf(L"Effect_D3D11: CreateVertexShader for %s failed: %s\n", mName.c_str(), err.ErrorMessage());
		return false;
	}
	D3D_DEBUG_NAME(mpVertexShader.GetInterfacePtr(), name().c_str());
	
	// create pixel shader
	hr = device.CreatePixelShader((const void *)(pData + hdr.lengthVS), hdr.lengthPS,
									NULL, &mpPixelShader.GetInterfacePtr());
	if (FAILED(hr)) {
		_com_error err(hr);
		debugWPrintf(L"Effect_D3D11: CreatePixelShader for %s failed: %s\n", mName.c_str(), err.ErrorMessage());
		return false;
	}
	D3D_DEBUG_NAME(mpPixelShader.GetInterfacePtr(), name().c_str());

	// Create the input layout for feeding vertex shader
	D3D11_INPUT_ELEMENT_DESC elements[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	UINT numElements = _countof(elements);

	hr = device.CreateInputLayout(elements, numElements,
								  (const void *)pData, hdr.lengthVS,
								  &mpInputLayout.GetInterfacePtr());
	if (FAILED(hr)) {
		_com_error err(hr);
		debugWPrintf(L"Effect_D3D11: CreateInputLayout for %s failed: %s\n", mName.c_str(), err.ErrorMessage());
		return false;
	}

	// shader reflection
	if (!reflectShader((const void *)pData, hdr.lengthVS) ||
		!reflectShader((const void *)(pData + hdr.lengthVS), hdr.lengthPS))
	{
		return false;
	}

	return true;
}

/*---------------------------------------------------------------------
	Run shader reflection to get input/output parameters
---------------------------------------------------------------------*/
bool Effect_D3D11::reflectShader(const void *pSrcData, size_t size)
{
	ID3D11ShaderReflection *pReflector = 0;
	
	HRESULT hr = D3DReflect(pSrcData, size,
							IID_ID3D11ShaderReflection,
							(void**)&pReflector);
	if (FAILED(hr)) {
		_com_error err(hr);
		debugWPrintf(L"Effect_D3D11: D3DReflect for %s failed: %s\n", mName.c_str(), err.ErrorMessage());
		return false;
	}

	pReflector->Release();

	return true;
}


#if defined(ICARUS_DEV_TOOLS)

/*---------------------------------------------------------------------
	Compile shaders from files on disk, save binaries to .ifx file
---------------------------------------------------------------------*/
bool Effect_D3D11::compileShadersToEffect(const wstring &filenameVS, const wstring &filenamePS,
										  const wstring &outFilename)
{
	ID3D11Device &device =
		static_cast<Renderer_D3D11*>(sRenderer.lock().get())->getDevice();

	// vars to hold compiled shaders
	ID3DBlob *pBlobVS = 0;
	ID3DBlob *pBlobPS = 0;
	ID3DBlob *pBlobError = 0;
	
	// set shader flags
	UINT dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
	#if defined(_DEBUG)
	dwShaderFlags |= D3DCOMPILE_DEBUG | D3DCOMPILE_WARNINGS_ARE_ERRORS;
	#else
	dwShaderFlags |= D3DCOMPILE_OPTIMIZATION_LEVEL3;
	#endif

	// set shader model
	LPCSTR targetVS = (device.GetFeatureLevel() >= D3D_FEATURE_LEVEL_11_0) ? "vs_5_0" : "vs_4_0";
	
	// set compiler macros
	const D3D_SHADER_MACRO defines[] = 
    {
        "EXAMPLE_DEFINE", "1",
        NULL, NULL
    };

	// open vertex shader file
	FILE *vsFile = _wfsopen(filenameVS.c_str(), L"rb", _SH_DENYWR);
	if (!vsFile) {
		debugWPrintf(L"Effect_D3D11: vertex shader file %s not found\n", filenameVS.c_str());
		return false;
	}
	// read data from file
	fseek(vsFile, 0, SEEK_END);
	long vsSize = ftell(vsFile);
	rewind(vsFile);
	unique_ptr<char[]> vsData(new char[vsSize]);
	size_t vsResult = fread(vsData.get(), 1, vsSize, vsFile);
	fclose(vsFile);
	if (vsResult != vsSize) {
		debugWPrintf(L"Effect_D3D11: vertex shader file %s read error\n", filenameVS.c_str());
		return false;
	}

	// compile and create the vertex shader
	HRESULT hr = D3DCompile(vsData.get(), vsSize, "VS",
							defines, NULL, "VSMain",
							targetVS, dwShaderFlags, 0, &pBlobVS, &pBlobError);
	if (FAILED(hr)) {
		if (pBlobError != NULL) {
			OutputDebugStringA((CHAR*)pBlobError->GetBufferPointer());
			pBlobError->Release();
		}
		return false;
	}
	ID3DBlobPtr vsBlobPtr;
	vsBlobPtr.Attach(pBlobVS, false);

	hr = device.CreateVertexShader(pBlobVS->GetBufferPointer(), pBlobVS->GetBufferSize(), 
									NULL, &mpVertexShader.GetInterfacePtr());
	if (FAILED(hr)) {
		_com_error err(hr);
		debugWPrintf(L"Effect_D3D11: CreateVertexShader for %s failed: %s\n", mName.c_str(), err.ErrorMessage());
		return false;
	}
	D3D_DEBUG_NAME(mpVertexShader.GetInterfacePtr(), "Default Vertex Shader");

	if (!reflectShader(pBlobVS->GetBufferPointer(), pBlobVS->GetBufferSize())) {
		return false;
	}

	// Compile and create the pixel shader
	LPCSTR targetPS = (device.GetFeatureLevel() >= D3D_FEATURE_LEVEL_11_0) ? "ps_5_0" : "ps_4_0";

	// open vertex shader file
	FILE *psFile = _wfsopen(filenamePS.c_str(), L"rb", _SH_DENYWR);
	if (!psFile) {
		debugWPrintf(L"Effect_D3D11: pixel shader file %s not found\n", filenamePS.c_str());
		return false;
	}
	// read data from file
	fseek(psFile, 0, SEEK_END);
	long psSize = ftell(psFile);
	rewind(psFile);
	unique_ptr<char[]> psData(new char[psSize]);
	size_t psResult = fread(psData.get(), 1, psSize, psFile);
	fclose(psFile);
	if (psResult != psSize) {
		debugWPrintf(L"Effect_D3D11: pixel shader file %s read error\n", filenamePS.c_str());
		return false;
	}

	hr = D3DCompile(psData.get(), psSize, "PS",
					defines, NULL, "PSMain",
					targetPS, dwShaderFlags, 0, &pBlobPS, &pBlobError);
	if (FAILED(hr)) {
		if (pBlobError != NULL) {
			OutputDebugStringA((CHAR*)pBlobError->GetBufferPointer());
			pBlobError->Release();
		}
		return false;
	}
	ID3DBlobPtr psBlobPtr;
	psBlobPtr.Attach(pBlobPS, false);

	hr = device.CreatePixelShader(pBlobPS->GetBufferPointer(), pBlobPS->GetBufferSize(),
									NULL, &mpPixelShader.GetInterfacePtr());
	if (FAILED(hr)) {
		_com_error err(hr);
		debugWPrintf(L"Effect_D3D11: CreatePixelShader for %s failed: %s\n", mName.c_str(), err.ErrorMessage());
		return false;
	}
	D3D_DEBUG_NAME(mpPixelShader.GetInterfacePtr(), "Default Pixel Shader");

	if (!reflectShader(pBlobPS->GetBufferPointer(), pBlobPS->GetBufferSize())) {
		return false;
	}

	// save .ifx file
	if (!saveToIfxFile(	pBlobVS->GetBufferPointer(),
						pBlobPS->GetBufferPointer(),
						(uint32_t)pBlobVS->GetBufferSize(),
						(uint32_t)pBlobPS->GetBufferSize(),
						outFilename))
	{
		return false;
	}

	// Create the input layout for feeding vertex shader
	D3D11_INPUT_ELEMENT_DESC elements[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	UINT numElements = _countof(elements);

	hr = device.CreateInputLayout(elements, numElements, pBlobVS->GetBufferPointer(),
									pBlobVS->GetBufferSize(), &mpInputLayout.GetInterfacePtr());
	if (FAILED(hr)) {
		_com_error err(hr);
		debugWPrintf(L"Effect_D3D11: CreateInputLayout for %s failed: %s\n", mName.c_str(), err.ErrorMessage());
		return false;
	}

	return true;
}

/*---------------------------------------------------------------------
	Saves a compiled shader to .ifx file format
---------------------------------------------------------------------*/
bool Effect_D3D11::saveToIfxFile(const void *pBufferVS, const void *pBufferPS,
								 uint32_t vsLength, uint32_t psLength,
								 const wstring &outFilename) const
{
	FILE *outFile = _wfsopen(outFilename.c_str(), L"wb", _SH_DENYNO);
	if (!outFile) {
		debugWPrintf(L"Effect_D3D11: failed to create output file %s\n", outFilename.c_str());
		return false;
	}
	IfxHeader hdr = {
		"ifx", 1,
		(uint32_t)vsLength,
		(uint32_t)psLength
	};

	fwrite(&hdr, 1, ifxHeaderSize, outFile);
	fwrite(pBufferVS, 1, hdr.lengthVS, outFile);
	fwrite(pBufferPS, 1, hdr.lengthPS, outFile);
	fclose(outFile);

	return true;
}

#endif

#endif

/* Renderer_d3d11.cpp
Author: Jeff Kiah
Orig.Date: 06/01/2012
*/
#include "Renderer_d3d11.h"
#include "Application/Win32/Win32.h"
#include "Render/Texture2D.h"
#include "Render/RenderBuffer.h"
#include "Render/Effect.h"
#include "Process/ThreadProcess.h"

#include "Math/Vector3f.h" // TEMP?
#include "Render/TextWriter.h" // TEMP

#if defined(WIN32)

#define MSAA_NUM_SAMPLES	1	// TODO replace this with a setting

// Renderer_D3D11 functions

bool Renderer_D3D11::initRenderer()
{
	return	initDeviceAndSwapChain() &&
			checkFeatureSupport() &&
			initDeviceObjects() &&
			initDeviceContext();
}

void Renderer_D3D11::render()
{
	// Clear the render target to black
	float clearColor[4] = { 0, 0, 0, 1.0f }; // rgba
	mpContext->ClearRenderTargetView(mpRenderTargetView, clearColor);

	// Render a triangle
	//mpContext->Draw(3, 0);

	if (!mTextWriter) {
		mTextWriter = new TextWriter_FW1();
		mTextWriter->init(L"Courier New", mpDevice);
	}

	mTextWriter->drawText(L"Hello World!", 16, 0, 0, 0xFFFFFFFF, mpContext);

	// Show the rendered frame on the screen
	mpSwapChain->Present(1, 0);
}

// Implementation functions

bool Renderer_D3D11::checkFeatureSupport()
{
	HRESULT hr = S_OK;
	D3D11_FEATURE_DATA_THREADING fdt;

	// check d3d driver threading feature support
	hr = mpDevice->CheckFeatureSupport(D3D11_FEATURE_THREADING, &fdt, sizeof(fdt));
	if (FAILED(hr)) {
		m_pPlatform->showErrorBox(L"D3D11 driver threading feature check failed");
		return false;
	}
	debugPrintf("Threading Feature:\n\tDriverConcurrentCreates: %s\n\tDriverCommandLists: %s\n",
		fdt.DriverConcurrentCreates ? "true" : "false", fdt.DriverCommandLists ? "true" : "false");

	// check multisample quality levels
	UINT msaaQualityLevels = 0;
	hr = mpDevice->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, MSAA_NUM_SAMPLES, &msaaQualityLevels);
	if (FAILED(hr)) {
		m_pPlatform->showErrorBox(L"D3D11 CheckMultisampleQualityLevels failed");
		return false;
	} else if (msaaQualityLevels == 0) {
		m_pPlatform->showErrorBox(L"MSAA sample count not supported");
		return false;
	}
	debugPrintf("MSAA:\n\tMultisamples per pixel: %i\n\tQuality levels: %i\n",
		MSAA_NUM_SAMPLES, msaaQualityLevels);

	return true;
}

bool Renderer_D3D11::initDeviceAndSwapChain()
{
	const Win32 *win32 = static_cast<const Win32*>(m_pPlatform);
	RECT rc;
	GetClientRect(win32->hWnd, &rc);
	m_viewportWidth = rc.right - rc.left;
	m_viewportHeight = rc.bottom - rc.top;
	debugPrintf("Client area width=%u height=%u\n", m_viewportWidth, m_viewportHeight);

	UINT createDeviceFlags = 0;
#ifdef _DEBUG
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#else
	//createDeviceFlags |= D3D11_CREATE_DEVICE_PREVENT_ALTERING_LAYER_SETTINGS_FROM_REGISTRY;
#endif
	
	const D3D_FEATURE_LEVEL featureLevels[] = {
		D3D_FEATURE_LEVEL_11_0
	};

	UINT numFeatureLevels = _countof(featureLevels);
	D3D_FEATURE_LEVEL featureLevelOut;

	DXGI_SWAP_CHAIN_DESC sd;
	SecureZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 2;
	sd.BufferDesc.Width = m_viewportWidth;
	sd.BufferDesc.Height = m_viewportHeight;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.SampleDesc.Count = MSAA_NUM_SAMPLES;	// multisampling
	sd.SampleDesc.Quality = D3D11_STANDARD_MULTISAMPLE_PATTERN;
	sd.OutputWindow = win32->hWnd;
	sd.Windowed = TRUE;

	HRESULT hr = D3D11CreateDeviceAndSwapChain(
					NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags,
					featureLevels, numFeatureLevels, D3D11_SDK_VERSION, 
					&sd, &mpSwapChain, &mpDevice, &featureLevelOut,
					&mpContext);
	if (FAILED(hr)) {
		m_pPlatform->showErrorBox(L"Failed to create D3D11 device");
		return false;
	}
	
	D3D_DEBUG_NAME(mpDevice, "Icarus D3D11 Device");
	D3D_DEBUG_NAME(mpContext, "Icarus D3D11 DeviceContext");
	D3D_DEBUG_NAME(mpSwapChain, "Icarus D3D11 SwapChain");
	
	return true;
}

bool Renderer_D3D11::initDeviceObjects()
{
	HRESULT hr = S_OK;
    
	// Create the render target view
	ID3D11Texture2DPtr pRenderTargetTexture;
	hr = mpSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pRenderTargetTexture.GetInterfacePtr());
	if (FAILED(hr)) {
		m_pPlatform->showErrorBox(L"Failed to create D3D11 render target texture");
		return false;
	}

	hr = mpDevice->CreateRenderTargetView(pRenderTargetTexture, NULL, &mpRenderTargetView.GetInterfacePtr());
	if (FAILED(hr)) {
		m_pPlatform->showErrorBox(L"Failed to create D3D11 render target view");
		return false;
	}
	D3D_DEBUG_NAME(mpRenderTargetView.GetInterfacePtr(), "Icarus D3D11 RenderTargetView");

	return true;
}

bool Renderer_D3D11::initDeviceContext()
{
//	mpContext->IASetInputLayout(mpInputLayout.GetInterfacePtr());

//	RenderBufferImpl *pBuf = static_cast<RenderBufferImpl *>(vBuf.get());
//	ID3D11Buffer *d3dBuf = pBuf->getBuffer().GetInterfacePtr();
	//mpContext->IASetVertexBuffers(0, 1, &d3dBuf, &stride, &offset);
	//mpContext->IASetIndexBuffer(0, 1, &d3dBuf, &stride, &offset);

//	mpContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

//	mpContext->VSSetShader(mpVertexShader.GetInterfacePtr(), NULL, 0);

//	mpContext->PSSetShader(mpPixelShader.GetInterfacePtr(), NULL, 0);

	D3D11_VIEWPORT vp;
	vp.Width = (FLOAT)m_viewportWidth;
	vp.Height = (FLOAT)m_viewportHeight;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	mpContext->RSSetViewports(1, &vp);

	mpContext->OMSetRenderTargets(1, &mpRenderTargetView.GetInterfacePtr(), NULL);

	return true;
}

void Renderer_D3D11::cleanup()
{
	delete mTextWriter;
}

// Constructor
RendererPtr Renderer_D3D11::create(const Platform *pPlatform)
{
	RendererPtr rndPtr(new Renderer_D3D11(pPlatform));
	// inject pointer to the dependent classes
	Texture2D_D3D11::sRenderer = rndPtr;
	RenderBuffer_D3D11::sRenderer = rndPtr;
	Effect_D3D11::sRenderer = rndPtr;

	return rndPtr;
}

Renderer_D3D11::Renderer_D3D11(const Platform *pPlatform) :
	Renderer_Base(pPlatform),
	mpSwapChain(0), mpDevice(0), mpContext(0),
	mpRenderTargetView(0),
	mTextWriter(0) // temp
{}


// RenderProcess
/*=====================================================================
class RenderProcess
=====================================================================*/
class Renderer_D3D11::RenderProcess : public ThreadProcess {
	private:
		ID3D11DeviceContextPtr	mpDeferredContext;
		Renderer_D3D11 &		mRenderer;

	public:
		// render process runs on a separate thread, builds command lists on deferred context
		// render process should have a list of RenderLayers
		// RenderLayer.getScene() has a pointer to camera for viewport
		// each RenderLayer has optional render target for render to texture, default is the screen buffer

		// the threadProc should wait for a render event, then run the process and produce a render result event
		// which includes the command list. The onUpdate in main thread gives the command list to the immediate renderer.
		// Once all expected processes produce a result, the frame is rendered by executing all command lists

		// render processes should support chaining
		// also support grouping so main thread knows when all parallel render processes are finished and the frame is ready to be rendered

		void threadProc();
		virtual void onUpdate(float deltaMillis);
		virtual bool onInitialize();

		explicit RenderProcess(Renderer_D3D11 &renderer, const string &name = "RenderProcess");
		virtual ~RenderProcess() {}
};

// This procedure builds the render command list in a separate thread
void Renderer_D3D11::RenderProcess::threadProc()
{
	//mpDeferredContext->Draw();
}

// This procedure runs on the main thread along with the immediate context
void Renderer_D3D11::RenderProcess::onUpdate(float deltaMillis)
{
	ID3D11CommandList* pd3dCommandList = 0;
	HRESULT hr = mpDeferredContext->FinishCommandList(FALSE, &pd3dCommandList);

	// this part definitely should NOT be called here, but is just for illustration for now
	// above builds the command list and this below executes it
	if (pd3dCommandList) {
		mRenderer.getImmediateContext()->ExecuteCommandList(pd3dCommandList, TRUE);
	}
}

bool Renderer_D3D11::RenderProcess::onInitialize()
{
	ID3D11DeviceContext* pDeferredContext = 0;

	HRESULT hr = mRenderer.getDevice()->CreateDeferredContext(0, &pDeferredContext);
	if (FAILED(hr)) {
		_com_error err(hr);
		debugPrintf("Renderer_D3D11: process %s failed to initialize deferred context: %s\n", mName.c_str(), err.ErrorMessage());
		return false;
	}

	mpDeferredContext.Attach(pDeferredContext, false);

	ThreadProcess::onInitialize();
	return true;
}

Renderer_D3D11::RenderProcess::RenderProcess(Renderer_D3D11 &renderer, const string &name) :
	ThreadProcess(name),
	mRenderer(renderer)
{}

#endif
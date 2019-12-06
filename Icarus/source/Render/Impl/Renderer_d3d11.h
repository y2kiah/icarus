/* Renderer_d3d11.inl
Author: Jeff Kiah
Orig.Date: 5/22/12
*/
#pragma once
#if defined(WIN32)

#include "Render/Renderer.h"
#include "D3D11_Defs.h"

class Renderer_D3D11;
class TextWriter_FW1;

typedef Renderer_Base<Renderer_D3D11>	Renderer;
typedef Renderer_D3D11					RendererImpl;
typedef shared_ptr<Renderer>			RendererPtr;
typedef weak_ptr<Renderer>				RendererWeakPtr;

class Renderer_D3D11 : public Renderer_Base<Renderer_D3D11>
{
	friend class Renderer_Base<Renderer_D3D11>;
	friend class RenderProcess;

	private:
		class RenderProcess;

		///// VARIABLES /////
		IDXGISwapChainPtr			mpSwapChain;
		ID3D11DevicePtr				mpDevice;
		ID3D11DeviceContextPtr		mpContext;
		ID3D11RenderTargetViewPtr	mpRenderTargetView; // temp
		TextWriter_FW1 *mTextWriter; // temp

		uint32_t	m_viewportWidth, m_viewportHeight;

		// Interface Functions
		bool initRenderer();
		void cleanup();
		void render();

		// Implementation Functions
		bool checkFeatureSupport();
		bool initDeviceAndSwapChain();
		bool initDeviceObjects();
		bool initDeviceContext();
	
	protected:
		explicit Renderer_D3D11(const Platform *pPlatform);

	public:
		// Accessors
		const ID3D11DevicePtr & getDevice() const { return mpDevice; }
		const ID3D11DeviceContextPtr & getImmediateContext() const { return mpContext; }

		// Constructor / destructor
		static RendererPtr create(const Platform *pPlatform);
		virtual ~Renderer_D3D11() {}
};

#endif
/* Effect_D3D11.h
Author: Jeff Kiah
Orig.Date: 06/24/2012
*/
#pragma once
#if defined(WIN32)

#include "Render/Effect.h"
#include "D3D11_Defs.h"
#include "Renderer_d3d11.h"

class Effect_D3D11 : public Effect_Base<Effect_D3D11> {
	friend class Effect_Base<Effect_D3D11>;
	friend class Renderer_D3D11;

	private:
		///// VARIABLES /////
		// Static
		static RendererWeakPtr	sRenderer;	// injected from renderer_d3d11 when it's created
		// Member
		ID3D11VertexShaderPtr	mpVertexShader;
		ID3D11PixelShaderPtr	mpPixelShader;
		ID3D11InputLayoutPtr	mpInputLayout;

		///// FUNCTIONS /////
		/*---------------------------------------------------------------------
			Run shader reflection to get input/output parameters
		---------------------------------------------------------------------*/
		bool reflectShader(const void *pSrcData, size_t size);

	public:
		/*---------------------------------------------------------------------
			return true for effect to initialize in init thread
		---------------------------------------------------------------------*/
		virtual bool useThreadInit() const;

		/*---------------------------------------------------------------------
			onLoad is called automatically by the resource caching system when
			a resource is first loaded from disk and added to the cache. This
			function loads pre-compiled shaders only. Expect file type .ifx
		---------------------------------------------------------------------*/
		virtual bool onLoad(const CharBufferPtr &dataPtr, bool async);
		
		/*---------------------------------------------------------------------
			onThreadInit performs initialization in the init thread after the
			resource bytes have been streamed from disk
		---------------------------------------------------------------------*/
		virtual bool onThreadInit(const CharBufferPtr &dataPtr);

		// Misc functions (tools build only)
#if defined(ICARUS_DEV_TOOLS)
		/*---------------------------------------------------------------------
			Compile shaders from files on disk, save binaries to .ifx file
		---------------------------------------------------------------------*/
		bool compileShadersToEffect(const wstring &filenameVS, const wstring &filenamePS,
									const wstring &outFilename);

		/*---------------------------------------------------------------------
			Saves a compiled shader to .ifx file format
		---------------------------------------------------------------------*/
		bool saveToIfxFile(const void *pBufferVS, const void *pBufferPS,
						   uint32_t vsLength, uint32_t psLength,
						   const wstring &outFilename) const;
#endif

		// Constructors
		explicit Effect_D3D11() :
			Effect_Base(),
			mpInputLayout(0), mpVertexShader(0), mpPixelShader(0)
		{}
		explicit Effect_D3D11(const wstring &name, size_t sizeB, const ResCachePtr &resCachePtr) :
			Effect_Base(name, sizeB, resCachePtr),
			mpInputLayout(0), mpVertexShader(0), mpPixelShader(0)
		{}
		virtual ~Effect_D3D11() {}
};

typedef Effect_Base<Effect_D3D11>	Effect;
typedef Effect_D3D11				EffectImpl;
typedef shared_ptr<Effect>			EffectPtr;

#endif

/* Effect_d3d11.h
Author: Jeff Kiah
Orig.Date: 06/02/2012
*/
#pragma once
#if defined(WIN32)

#include "Render/Effect/Effect.h"
#include "Render/Impl/Defines_d3d11.h"
#include <d3ddx11effect.h>

///// STRUCTURES /////

// forward declarations
class CheckResourceLoadedProcess;
struct ID3DXEffect;
struct ID3DXEffectPool;

class Matrix4x4f;
class Vector3f;
class Vector4f;
class Texture_D3D11;

/*=============================================================================
class Effect_D3D11
=============================================================================*/
class Effect_D3D11 : public Effect_Base<Effect_D3D11> {
	//friend class RenderManager_D3D9;	// allows access to spEffectPool
	//friend class CheckResourceLoadedProcess;

	private:
		/*=====================================================================
		class CheckResourceLoadedProcess
			A list of spawned processes is maintained for the life of the
			effect to protect against dereferencing a bad pointer in this
			process. If the effect is destroyed first, it will call finish()
			on all processes in the list that aren't already finished.
		=====================================================================*/
		class CheckResourceLoadedProcess : public Process {
			private:
				wstring			mResourcePath;
				D3DXHANDLE		mParamHandle;
				Effect_D3D11 *	mpEffect;

			public:
				virtual void onUpdate(float deltaMillis);
				virtual void onInitialize() {}
				virtual void onFinish() {}
				virtual void onTogglePause() {}

				explicit CheckResourceLoadedProcess(Effect_D3D11 *pEff, D3DXHANDLE paramHandle,
													const wstring &resourcePath);
				virtual ~CheckResourceLoadedProcess() {}
		};

		///// VARIABLES /////
		ID3DXEffect *		mD3DEffect;		// effect interface
		D3DXHANDLE			mhTechnique;	// handle to the effect's technique used to render
		D3DXEFFECT_DESC		mEffectDesc;	// effect description structure
		D3DXTECHNIQUE_DESC	mTechniqueDesc;	// description of the technique chosen at load time

		D3DXHANDLE			mMatrixHandle[MatrixHndl_MAX];	// handles for fast access to properties with
		D3DXHANDLE			mVectorHandle[VectorHndl_MAX];	// DXSAS compliant semantics
		D3DXHANDLE			mScalarHandle[ScalarHndl_MAX];
		D3DXHANDLE			mTextureHandle[TextureHndl_MAX];// see CPP file comments in parseKnownHandles for
															// special-case texture semantics

		// static
		static ID3DXEffectPool *	spEffectPool; // make smart (weak) pointer

		///// FUNCTIONS /////
		/*---------------------------------------------------------------------
			Called in a lost device situation for non-managed textures only.
			These will free and restore the resource.
		---------------------------------------------------------------------*/
		virtual void	onDeviceLost();
		virtual void	onDeviceRestore();

		/*---------------------------------------------------------------------
			This function finds known variables with predetermined names and
			stores a handle to them for faster access. The recognized semantics
			from the .fx file follows the DXSAS 0.86 spec. Known handles can be
			found in the CPP file. Returns true if initialization succeeds,
			false on failure.
		---------------------------------------------------------------------*/
		bool	parseKnownHandles();

		/*---------------------------------------------------------------------
			Takes an (assumed) valid parameter handle and description and loops
			through the parameter's annotations looking for recognized values.
			Child resources will be loaded when found.
			-- if both ResourceName and SourceName are found, it attempts to
				load from a registered IResourceSource object (async or sync)
		---------------------------------------------------------------------*/
		bool	parseParamAnnotations(D3DXHANDLE paramHndl, const D3DXPARAMETER_DESC &paramDesc);

		/*---------------------------------------------------------------------
			Called from parseParamAnnotations. Returns true if load successful,
			or for asynchronous loading, if waiting for load to complete.
			Returns false for synchronous loading when load fails, or on error
			for asynchronous loading.
		---------------------------------------------------------------------*/
		bool	loadTexture(D3DXHANDLE paramHndl, const string &resourcePath, D3DXPARAMETER_TYPE textureType);
		
		/*---------------------------------------------------------------------
		---------------------------------------------------------------------*/
		bool	loadScript(D3DXHANDLE paramHndl, const string &resourcePath);

	public:
		///// VARIABLES /////
		/*---------------------------------------------------------------------
			specifies the cache that will manager the resource
		---------------------------------------------------------------------*/
		static const ResCacheType	sCacheType = ResCache_Material;

		///// FUNCTIONS /////
		// Accessors
		/*---------------------------------------------------------------------
			Check that this returns true before using the effect for rendering.
		---------------------------------------------------------------------*/
		bool	isReadyForRender() const { return (mInitialized && (mResourceCount == mResources.size())); }

		// Mutators
		/*---------------------------------------------------------------------
			Call begin to render using the effect. Returns the number of passes
			or 0 on error. The client should loop the specified number of times
			returned and call beginPass / endPass for each iteration. endPass
			is called when finished rendering with this effect.
		---------------------------------------------------------------------*/
		uint32_t	beginEffect() const;
		void		endEffect() const;
		void		beginPass(uint32_t pass) const;
		void		endPass() const;

		/*---------------------------------------------------------------------
			These functions set effect parameters according to type and pre-
			determined handle
		---------------------------------------------------------------------*/
		bool	setMatrixParam(MatrixHandle h, const Matrix4x4f &m) const;
		bool	setVectorParam(VectorHandle h, const Vector3f &v) const;
		bool	setVectorParam(VectorHandle h, const Vector4f &v) const;
		bool	setScalarParam(ScalarHandle h, float f) const;
		bool	setScalarParam(ScalarHandle h, int i) const;
		bool	setScalarParam(ScalarHandle h, bool b) const;
		bool	setTextureParam(uint32_t i, const Texture_D3D11 *t) const;

		/*---------------------------------------------------------------------
			Loads an effect directly from a .fx file (not through the Resource-
			Source interface. Use this with the cache injection method.
		---------------------------------------------------------------------*/
		bool	loadFromFile(const wstring &filename);

		/*---------------------------------------------------------------------
			onLoad is called automatically by the resource caching system when
			a resource is first loaded from disk and added to the cache.
		---------------------------------------------------------------------*/
		virtual bool	onLoad(const BufferPtr &dataPtr, bool async);

		/*---------------------------------------------------------------------
			releases the D3D9 effect resource and sets mInitialized false
		---------------------------------------------------------------------*/
		void	clearEffectData();

		// Constructors
		/*---------------------------------------------------------------------
			constructor with this signature is required for the resource system
		---------------------------------------------------------------------*/
		explicit Effect_D3D11(const wstring &name, size_t sizeB, const ResCachePtr &resCachePtr);

		/*---------------------------------------------------------------------
			use this default constructor to create the texture without caching,
			or for cache injection method.
		---------------------------------------------------------------------*/
		explicit Effect_D3D11();

		// Destructor
		~Effect_D3D11();
};

typedef Effect_Base<Effect_D3D11> Effect;
typedef Effect_D3D11 EffectImpl;

#endif
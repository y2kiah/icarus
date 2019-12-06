/* Effect.h
Author: Jeff Kiah
Orig.Date: 06/02/2012
*/
#pragma once

#include <cstdint>
#include "Resource/ResHandle.h"
#include <string>
#include <vector>
#include <memory>

using std::vector;
using std::shared_ptr;
using std::string;
using std::wstring;

/*=============================================================================
class Effect_Base
=============================================================================*/
template <class Implementation>
class Effect_Base : public Resource {
	//friend class RenderManager_D3D9;	// allows access to spEffectPool
	//friend class CheckResourceLoadedProcess;

	public:
		///// DEFINITIONS /////
		typedef vector<ResPtr>			ResPtrList;
		typedef vector<D3DXSEMANTIC>	VertexInputList;

		// enums of SAS semantic names for indexing handle tables
		enum MatrixHandle : int {
			MatrixHndl_world = 0,		MatrixHndl_worldInv,		MatrixHndl_worldTrans,			MatrixHndl_worldInvTrans,
			MatrixHndl_view,			MatrixHndl_viewInv,			MatrixHndl_viewTrans,			MatrixHndl_viewInvTrans,
			MatrixHndl_proj,			MatrixHndl_projInv,			MatrixHndl_projTrans,			MatrixHndl_projInvTrans,
			MatrixHndl_worldView,		MatrixHndl_worldViewInv,	MatrixHndl_worldViewTrans,		MatrixHndl_worldViewInvTrans,
			MatrixHndl_viewProj,		MatrixHndl_viewProjInv,		MatrixHndl_viewProjTrans,		MatrixHndl_viewProjInvTrans,
			MatrixHndl_worldViewProj,	MatrixHndl_worldViewProjInv,MatrixHndl_worldViewProjTrans,	MatrixHndl_worldViewProjInvTrans,
			MatrixHndl_MAX
		};
		enum VectorHandle : int {
			VectorHndl_lightPos = 0,	VectorHndl_lightDir,		VectorHndl_diffuse,				VectorHndl_specular,
			VectorHndl_ambient,			VectorHndl_emissive,		VectorHndl_viewportPixelSize,
			VectorHndl_MAX
		};
		enum ScalarHandle : int {
			ScalarHndl_power = 0,		ScalarHndl_specularPower,	ScalarHndl_constAtten,			ScalarHndl_linearAtten,
			ScalarHndl_quadAtten,		ScalarHndl_falloffAngle,	ScalarHndl_falloffExponent,		ScalarHndl_emission,
			ScalarHndl_opacity,			ScalarHndl_refraction,
			ScalarHndl_MAX
		};
		enum TextureHandle : int {
			TextureHndl_rndrDSTarget=0,	TextureHndl_rndrClrTarget,	TextureHndl_diffuseMap,			TextureHndl_specularMap,
			TextureHndl_normalMap,		TextureHndl_environment,	TextureHndl_envMap,				TextureHndl_envNormal,
			TextureHndl_MAX
		};
		
		static const char *sSASMatrixSemanticNames[];
		static const char *sSASVectorSemanticNames[];
		static const char *sSASScalarSemanticNames[];
		static const char *sSASTextureSemanticNames[];

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

	protected:
		/*=====================================================================
		class CheckResourceLoadedProcess
			A list of spawned processes is maintained for the life of the
			effect to protect against dereferencing a bad pointer in this
			process. If the effect is destroyed first, it will call finish()
			on all processes in the list that aren't already finished.
		=====================================================================*/
		class CheckResourceLoadedProcess : public Process {
			private:
				string			mResourcePath;
				D3DXHANDLE		mParamHandle;
				Effect_D3D11 *	mpEffect;

			public:
				virtual void onUpdate(float deltaMillis);
				virtual void onInitialize() {}
				virtual void onFinish() {}
				virtual void onTogglePause() {}

				explicit CheckResourceLoadedProcess(Effect_D3D11 *pEff, D3DXHANDLE paramHandle,
													const string &resourcePath);
				virtual ~CheckResourceLoadedProcess() {}
		};

		///// VARIABLES /////
		ResPtrList			mResources;	// optional list of resources (textures, scripts) that are specified in the
										// .fx file annotations. These can be overridden by material-specific textures,
										// but can act as proxy textures, or for any effect-constant textures where the
										// texture semantic is not set to a pre-determined value
		VertexInputList		mVertexSemantics;	// vector of D3DXSEMANTIC, determines input format for vertex shader
												// http://msdn.microsoft.com/en-us/library/bb172872%28VS.85%29.aspx
		bool		mInitialized;	// true if initialization succeeds
		bool		mLoadAsync;		// true if onLoad passed 'true' in async, load child resources asynchronously
		int			mResourceCount;	// number of child resources (textures, scripts) found in parameter annotations
		ProcessList	mProcessList;	// list of processes spawned to load child resources

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

		// Constructors
		/*---------------------------------------------------------------------
			constructor with this signature is required for the resource system
		---------------------------------------------------------------------*/
		explicit Effect_Base(const wstring &name, size_t sizeB, const ResCachePtr &resCachePtr) :
			Resource(name, sizeB, resCachePtr),
			mInitialized(false), mLoadAsync(false), mResourceCount(0)
		{}

		/*---------------------------------------------------------------------
			use this default constructor to create the texture without caching,
			or for cache injection method.
		---------------------------------------------------------------------*/
		explicit Effect_Base() :
			Resource(),
			mInitialized(false), mLoadAsync(false), mResourceCount(0)
		{}

		// Destructor
		virtual ~Effect_Base() {}
};

#if defined(WIN32)
#include "Impl/Effect_d3d11.h"
#endif
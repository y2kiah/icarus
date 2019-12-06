/* Effect_d3d11.cpp
Author: Jeff Kiah
Orig.Date: 06/02/2012
*/

#include "Effect_D3D11.h"
#include "Utility/Debug.h"
#include "Render/Texture2D.h"
#include "Math/Vector3f.h"
//#include "../Scripting/ScriptFile_Lua.h"

ID3DXEffectPool * Effect_D3D11::spEffectPool = 0;

const char *Effect_D3D11::sSASMatrixSemanticNames[MatrixHndl_MAX] = {
	"WORLD",				"WORLDINVERSE",					"WORLDTRANSPOSE",				"WORLDINVERSETRANSPOSE",
	"VIEW",					"VIEWINVERSE",					"VIEWTRANSPOSE",				"VIEWINVERSETRANSPOSE",
	"PROJECTION",			"PROJECTIONINVERSE",			"PROJECTIONTRANSPOSE",			"PROJECTIONINVERSETRANSPOSE",
	"WORLDVIEW",			"WORLDVIEWINVERSE",				"WORLDVIEWTRANSPOSE",			"WORLDVIEWINVERSETRANSPOSE",
	"VIEWPROJECTION",		"VIEWPROJECTIONINVERSE",		"VIEWPROJECTIONTRANSPOSE",		"VIEWPROJECTIONINVERSETRANSPOSE",
	"WORLDVIEWPROJECTION",	"WORLDVIEWPROJECTIONINVERSE",	"WORLDVIEWPROJECTIONTRANSPOSE",	"WORLDVIEWPROJECTIONINVERSETRANSPOSE",
};
const char *Effect_D3D11::sSASVectorSemanticNames[VectorHndl_MAX] = {
	"POSITION",				"DIRECTION",					"DIFFUSE",						"SPECULAR",
	"AMBIENT",				"EMISSIVE",						"VIEWPORTPIXELSIZE"
};
const char *Effect_D3D11::sSASScalarSemanticNames[ScalarHndl_MAX] = {
	"POWER",				"SPECULARPOWER",				"CONSTANTATTENUATION",			"LINEARATTENUATION",
	"QUADRATICATTENUATION",	"FALLOFFANGLE",					"FALLOFFEXPONENT",				"EMISSION",
	"OPACITY",				"REFRACTION"
};
const char *Effect_D3D11::sSASTextureSemanticNames[TextureHndl_MAX] = {
	"RENDERDEPTHSTENCILTARGET",	"RENDERCOLORTARGET",		"DIFFUSEMAP",					"SPECULARMAP",
	"NORMAL",				"ENVIRONMENT",					"ENVMAP",						"ENVIRONMENTNORMAL"
};

///// FUNCTIONS /////

/*---------------------------------------------------------------------
	Call begin to render using the effect. Returns the number of passes
	or 0 on error. The client should loop the specified number of times
	returned and call beginPass / endPass for each iteration. endPass
	is called when finished rendering with this effect.
---------------------------------------------------------------------*/
uint32_t Effect_D3D11::beginEffect() const
{
	// Apply the technique contained in the effect
	uint32_t passes;
	if (SUCCEEDED(mD3DEffect->Begin(&passes, 0))) {
		return passes;
	}
	return 0; // Begin failed
}

void Effect_D3D11::endEffect() const
{
	mD3DEffect->End();
}

void Effect_D3D11::beginPass(uint32_t pass) const
{
	mD3DEffect->BeginPass(pass);
}

void Effect_D3D11::endPass() const
{
	mD3DEffect->EndPass();
}

/*---------------------------------------------------------------------
	These functions set effect parameters according to type and pre-
	determined handle
---------------------------------------------------------------------*/
bool Effect_D3D11::setMatrixParam(MatrixHandle h, const Matrix4x4f &m) const { return true; }
bool Effect_D3D11::setVectorParam(VectorHandle h, const Vector3f &v) const { return true; }
bool Effect_D3D11::setVectorParam(VectorHandle h, const Vector4f &v) const { return true; }
bool Effect_D3D11::setScalarParam(ScalarHandle h, float f) const { return true; }
bool Effect_D3D11::setScalarParam(ScalarHandle h, int i) const { return true; }
bool Effect_D3D11::setScalarParam(ScalarHandle h, bool b) const { return true; }
bool Effect_D3D11::setTextureParam(uint32_t i, const Texture_D3D11 *t) const { return true; }

/*---------------------------------------------------------------------
	Loads an effect directly from a .fx file (not through the Resource-
	Source interface. Use this with the cache injection method.
---------------------------------------------------------------------*/
bool Effect_D3D11::loadFromFile(const wstring &filename)
{
	DWORD dwShaderFlags = D3DXFX_NOT_CLONEABLE;

	// D3DXCreateEffectFromFile can not load a relative path
	char fullPath[MAX_PATH];
	_fullpath(fullPath, filename.c_str(), MAX_PATH);

	HRESULT hr;
	hr = D3DXCreateEffectFromFileA(
			spD3DDevice,
			fullPath,
			NULL,
			NULL,
			dwShaderFlags,
			spEffectPool,
			&mD3DEffect,
			NULL);
	if (FAILED(hr)) {
		d3dDebugSwitch(hr);
		debugPrintf("Effect_D3D9: Error: cannot create effect \"%s\"\n", filename.c_str());
		return false;
	}
	mInitFlags[INIT_EFFECT] = true; // set the init flag

	// Get the Effect description
	mD3DEffect->GetDesc(&mEffectDesc);

	mName = filename; // sets the name in the resource base class to the filename

	// Get the first valid technique, starting at top of file. Techniques
	// in the file are assumed to be ordered by most demanding to least
	hr = mD3DEffect->FindNextValidTechnique(NULL, &mhTechnique);
	if (FAILED(hr) || mhTechnique == NULL) {
		d3dDebugSwitch(hr);
		debugPrintf("Effect_D3D9: Error: no valid technique in \"%s\"\n", filename.c_str());
		return false;
	}
	// Get some info about the technique
	hr = mD3DEffect->GetTechniqueDesc(mhTechnique, &mTechniqueDesc);
    if (FAILED(hr)) {
		d3dDebugSwitch(hr);
		debugPrintf("Effect_D3D9: Error: GetTechniqueDesc failed in \"%s\"\n", filename.c_str());
		return false;
	}
	// Set the determined technique to be used for this effect every time
	hr = mD3DEffect->SetTechnique(mhTechnique);
	if (FAILED(hr)) {
		d3dDebugSwitch(hr);
		debugPrintf("Effect_D3D9: Error: SetTechnique failed in \"%s\"\n", filename.c_str());
		return false;
	}
	mInitFlags[INIT_TECHNIQUE] = true; // set the init flag

	if (!parseKnownHandles()) {
		debugPrintf("Effect_D3D9: Error: failed to load child resources in \"%s\"\n", filename.c_str());
		return false;
	}

	setIsManaged(false);
	mInitialized = true;

	return true;
}

/*---------------------------------------------------------------------
	onLoad is called automatically by the resource caching system when
	a resource is first loaded from disk and added to the cache.
---------------------------------------------------------------------*/
bool Effect_D3D11::onLoad(const BufferPtr &dataPtr, bool async)
{
	mLoadAsync = async;	// mLoadAsync is used when parsing annotations for child resources to load
	DWORD dwShaderFlags = D3DXFX_NOT_CLONEABLE;

	HRESULT hr;
	hr = D3DXCreateEffect(
			spD3DDevice,
			(LPCVOID)dataPtr.get(),
			sizeB(),
			NULL,
			NULL,
			dwShaderFlags,
			spEffectPool,
			&mD3DEffect,
			NULL);
	if (FAILED(hr)) {
		d3dDebugSwitch(hr);
		debugPrintf("Effect_D3D9: Error: cannot create effect \"%s\"\n", name().c_str());
		return false;
	}
	mInitFlags[INIT_EFFECT] = true; // set the init flag

	// get the description
	mD3DEffect->GetDesc(&mEffectDesc);

	// Get the first valid technique, starting at top of file. Techniques
	// in the file are assumed to be ordered by most demanding to least
	hr = mD3DEffect->FindNextValidTechnique(NULL, &mhTechnique);
	if (FAILED(hr) || mhTechnique == NULL) {
		d3dDebugSwitch(hr);
		debugPrintf("Effect_D3D9: Error: no valid technique in \"%s\"\n", name().c_str());
		return false;
	}
	// Get some info about the technique
	hr = mD3DEffect->GetTechniqueDesc(mhTechnique, &mTechniqueDesc);
    if (FAILED(hr)) {
		d3dDebugSwitch(hr);
		debugPrintf("Effect_D3D9: Error: GetTechniqueDesc failed in \"%s\"\n", name().c_str());
		return false;
	}
	// Set the determined technique to be used for this effect every time
	hr = mD3DEffect->SetTechnique(mhTechnique);
	if (FAILED(hr)) {
		d3dDebugSwitch(hr);
		debugPrintf("Effect_D3D9: Error: SetTechnique failed in \"%s\"\n", name().c_str());
		return false;
	}
	mInitFlags[INIT_TECHNIQUE] = true; // set the init flag
	
	if (!parseKnownHandles()) {
		debugPrintf("Effect_D3D9: Error: failed to load child resources in \"%s\"\n", name().c_str());
		return false;
	}

	setIsManaged(false);
	mInitialized = true;

	return true;
}

/*---------------------------------------------------------------------
	This function finds known variables with predetermined names and
	stores a handle to them for faster access. The recognized semantics
	from the .fx file follows the DXSAS 0.86 spec. Returns true if
	initialization succeeds, false on failure.
---------------------------------------------------------------------*/
bool Effect_D3D11::parseKnownHandles()
{
	debugPrintf("Parsing \"%s\" for known handles:\n", name().c_str());
	// Cycle through parameters
	for (UINT p = 0; p < mEffectDesc.Parameters; ++p) {
		D3DXHANDLE hParam = mD3DEffect->GetParameter(NULL, p);
		D3DXPARAMETER_DESC paramDesc;
		mD3DEffect->GetParameterDesc(hParam, &paramDesc);

		debugPrintf("  ..checking Name:\"%s\" Semantic:\"%s\" Class:%i Type:%i\n", paramDesc.Name, paramDesc.Semantic, paramDesc.Class, paramDesc.Type);

		if (paramDesc.Semantic != NULL) {
			if (paramDesc.Class == D3DXPC_MATRIX_ROWS ||
				paramDesc.Class == D3DXPC_MATRIX_COLUMNS)
			{
				for (int i = 0; i < MatrixHndl_MAX; ++i) {
					if (_strcmpi(paramDesc.Semantic, sSASMatrixSemanticNames[i]) == 0) {
						mMatrixHandle[i] = hParam;
						debugPrintf("  found matrix semantic \"%s\"\n", paramDesc.Semantic);
						break;
					}
				}
			} else if (paramDesc.Class == D3DXPC_VECTOR) {
				for (int i = 0; i < VectorHndl_MAX; ++i) {
					if (_strcmpi(paramDesc.Semantic, sSASVectorSemanticNames[i]) == 0) {
						mVectorHandle[i] = hParam;
						debugPrintf("  found vector semantic \"%s\"\n", paramDesc.Semantic);
						break;
					}
				}
			} else if (paramDesc.Class == D3DXPC_SCALAR) {
				for (int i = 0; i < ScalarHndl_MAX; ++i) {
					if (_strcmpi(paramDesc.Semantic, sSASScalarSemanticNames[i]) == 0) {
						mScalarHandle[i] = hParam;
						debugPrintf("  found scalar semantic \"%s\"\n", paramDesc.Semantic);
						break;
					}
				}
			} else if (paramDesc.Class == D3DXPC_OBJECT) {
				if (paramDesc.Type == D3DXPT_TEXTURE ||
					paramDesc.Type == D3DXPT_TEXTURE1D ||
					paramDesc.Type == D3DXPT_TEXTURE2D ||
					paramDesc.Type == D3DXPT_TEXTURE3D ||
					paramDesc.Type == D3DXPT_TEXTURECUBE)
				{
					// texture handles will recognize the SAS semantics, and also semantics with the
					// pattern TEXTURE# where # is an integer from 0 to TextureHndl_MAX-1. For example,
					// semantics TEXTURE3 and DIFFUSEMAP will resolve to the same handle.
					string sStr(paramDesc.Semantic);
					if (_stricmp(sStr.substr(0, 7).c_str(), "texture") == 0) {
						int iPos = sStr.find_first_of("0123456789", 7, 2);
						if (iPos != string::npos) {
							int iTex = atoi(&paramDesc.Semantic[iPos]);
							if (iTex >= 0 && iTex < TextureHndl_MAX) {
								mTextureHandle[iTex] = hParam;
								debugPrintf("  found texture semantic \"%s\"\n", paramDesc.Semantic);
							}
						}
					} else {
						for (int i = 0; i < TextureHndl_MAX; ++i) {
							if (_strcmpi(paramDesc.Semantic, sSASTextureSemanticNames[i]) == 0) {
								mTextureHandle[i] = hParam;
								debugPrintf("  found texture semantic \"%s\"\n", paramDesc.Semantic);
								break;
							}
						}
					}
				}
			}
		}
		// Cycle through the parameter's annotations, if a child resource fails to load
		// (synchronous loading only) immediately return false since this effect won't work
		if (!parseParamAnnotations(hParam, paramDesc)) { return false; }
	}
	return true;
}

/*---------------------------------------------------------------------
	Takes an (assumed) valid parameter handle and description and loops
	through the parameter's annotations looking for recognized values.
	Child resources will be loaded when found.
	-- if both ResourceName and SourceName are found, it attempts to
		load from a registered IResourceSource object (async or sync)
---------------------------------------------------------------------*/
bool Effect_D3D11::parseParamAnnotations(D3DXHANDLE paramHndl, const D3DXPARAMETER_DESC &paramDesc)
{
	// Handle texture annotations
	// for any kind of texture type, we'll look for specific textures being requested
	if (paramDesc.Type == D3DXPT_TEXTURE || paramDesc.Type == D3DXPT_TEXTURE1D ||
		paramDesc.Type == D3DXPT_TEXTURE2D || paramDesc.Type == D3DXPT_TEXTURE3D ||
		paramDesc.Type == D3DXPT_TEXTURECUBE)
	{
		// The texture semantics "RENDERDEPTHSTENCILTARGET" and "RENDERCOLORTARGET" tell the engine
		// to create a render target for the effect. Use the resourcename annotation to name the texture
		// so other effects can use the results of the render.
		if ((paramDesc.Type == D3DXPT_TEXTURE || paramDesc.Type == D3DXPT_TEXTURE2D) &&
			(_strcmpi(paramDesc.Semantic, sSASScalarSemanticNames[TextureHndl_rndrDSTarget]) == 0 ||
			 _strcmpi(paramDesc.Semantic, sSASScalarSemanticNames[TextureHndl_rndrClrTarget]) == 0))
		{
			D3DXHANDLE aHndl = NULL;
			D3DXPARAMETER_DESC aDesc;

			// unlike the DXSAS spec, which states that resourcename is meaningless for render targets, we
			// name the resource to enable texture sharing among effects (another effect later in the
			// pipeline need only request a texture of the same unique name, and it will be retrieved from
			// the cache)
			const char *resNameValueStr = 0;
			if (aHndl = mD3DEffect->GetAnnotationByName(paramHndl, "ResourceName")) {
				if (FAILED(mD3DEffect->GetString(aHndl, &resNameValueStr))) {
					// let the resource system assign a unique name
				}
				debugPrintf("    found annotation: string ResourceName = \"%s\"\n", resNameValueStr);
			}
			// Get the other parameters
			// ViewportRatio, Dimensions, and Width/Height/Depth are mutually exclusive.
			// Dimensions will override ViewportRatio, and Width/Height/Depth will override Dimensions
			Vector2f viewportRatio(1.0f, 1.0f);
			if (aHndl = mD3DEffect->GetAnnotationByName(paramHndl, "ViewportRatio")) {
				mD3DEffect->GetFloatArray(aHndl, viewportRatio.v, 2);
				debugPrintf("    found annotation: float2 ViewportRatio = {%0.3f,%0.3f}\n", viewportRatio.x, viewportRatio.y);
			}
			Vector3i dimensions(0, 0, 0);
			if (aHndl = mD3DEffect->GetAnnotationByName(paramHndl, "Dimensions")) {
				mD3DEffect->GetParameterDesc(aHndl, &aDesc);
				mD3DEffect->GetIntArray(aHndl, dimensions.v, aDesc.Elements);
				debugPrintf("    found annotation: int%u Dimensions = {%i,%i,%i}\n", aDesc.Elements, dimensions.x, dimensions.y, dimensions.z);
			}
			if (aHndl = mD3DEffect->GetAnnotationByName(paramHndl, "Width")) {
				mD3DEffect->GetInt(aHndl, &dimensions.x);
				debugPrintf("    found annotation: int Width = %i\n", dimensions.x);
			}
			if (aHndl = mD3DEffect->GetAnnotationByName(paramHndl, "Height")) {
				mD3DEffect->GetInt(aHndl, &dimensions.y);
				debugPrintf("    found annotation: int Height = %i\n", dimensions.y);
			}
			if (aHndl = mD3DEffect->GetAnnotationByName(paramHndl, "Depth")) {
				mD3DEffect->GetInt(aHndl, &dimensions.z);
				debugPrintf("    found annotation: int Depth = %i\n", dimensions.z);
			}

			const char *formatStr = 0;
			if (aHndl = mD3DEffect->GetAnnotationByName(paramHndl, "Format")) {
				mD3DEffect->GetString(aHndl, &formatStr);
				debugPrintf("    found annotation: string Format = \"%s\"\n", formatStr);
			}

			int mipLevels = 0;
			if (aHndl = mD3DEffect->GetAnnotationByName(paramHndl, "MipLevels")) {
				mD3DEffect->GetInt(aHndl, &mipLevels);
				debugPrintf("    found annotation: int MipLevels = %i\n", mipLevels);
			} else if (aHndl = mD3DEffect->GetAnnotationByName(paramHndl, "Levels")) {
				mD3DEffect->GetInt(aHndl, &mipLevels);
				debugPrintf("    found annotation: int Levels = %i\n", mipLevels);
			}

			const char *functionStr = 0;
			if (aHndl = mD3DEffect->GetAnnotationByName(paramHndl, "Function")) {
				mD3DEffect->GetString(aHndl, &functionStr);
				debugPrintf("    found annotation: string Function = \"%s\"\n", functionStr);
			}

			// create the render target here
			//++mResourceCount;	// increment child resource count, size of mResource list must match
								// this value for the effect to be ready for rendering
			//bool result = loadTexture(paramHndl, resNameValueStr);
			//return result;

		// Non render target textures, loaded from cache if the resource name is present
		} else {
			// The following section finds a texture annotation named "resourcename" and attempts
			// to load and set the texture. Loading will follow the asynchronous pattern if appropriate,
			// and a process will be spawned to check for the load completion. For synchronous loading,
			// all textures will be immediately fetched. False is returned if a resource cannot be loaded.
			D3DXHANDLE resNameHndl = mD3DEffect->GetAnnotationByName(paramHndl, "ResourceName");
			if (!resNameHndl) { return true; }
			const char *resNameValueStr = 0;
			if (SUCCEEDED(mD3DEffect->GetString(resNameHndl, &resNameValueStr))) {
				debugPrintf("    found annotation: string ResourceName = \"%s\"\n", resNameValueStr);
				
				// find the texture type expected by the effect
				D3DXPARAMETER_TYPE textureType = (paramDesc.Type == D3DXPT_TEXTURE ?
												  D3DXPT_TEXTURE2D : paramDesc.Type);
				// If the texture type is not provided by the variable type, the "resourcetype" annotation
				// will clue us in. If it's not present, just assume the default 2D. If the variable type
				// (1D, 2D, 3D, or CUBE) is specified, this annotation is ignored.
				if (paramDesc.Type == D3DXPT_TEXTURE) {
					D3DXHANDLE resTypeHndl = mD3DEffect->GetAnnotationByName(paramHndl, "ResourceType");
					if (resTypeHndl) {
						const char *resTypeValueStr = 0;
						if (SUCCEEDED(mD3DEffect->GetString(resTypeHndl, &resTypeValueStr))) {
							debugPrintf("    found annotation: string ResourceType = \"%s\"\n", resTypeValueStr);
							// find the texture type
							if		(_stricmp(resTypeValueStr, "cube") == 0) { textureType = D3DXPT_TEXTURECUBE; }
							else if (_stricmp(resTypeValueStr, "3d") == 0)	 { textureType = D3DXPT_TEXTURE3D; }
						}
					}
				}

				// load the texture here
				++mResourceCount;	// increment child resource count, size of mResource list must match
									// this value for the effect to be ready for rendering
				bool result = loadTexture(paramHndl, resNameValueStr, textureType);
				return result;
			}
		}
	}
	return true;
}

/*---------------------------------------------------------------------
	Called from parseParamAnnotations. Returns true if load successful,
	or for asynchronous loading, if waiting for load to complete.
	Returns false for synchronous loading when load fails, or on error
	for asynchronous loading.
---------------------------------------------------------------------*/
bool Effect_D3D11::loadTexture(D3DXHANDLE paramHndl, const string &resourcePath,
							  D3DXPARAMETER_TYPE textureType)
{
	if (mLoadAsync) {
		// for asynchronous loading, try to load the resource
		ResHandle h;
		ResLoadResult retVal = h.tryLoad<Texture_D3D9>(resourcePath);
		if (retVal == ResLoadResult_Success) {
			Texture_D3D9 *tex = static_cast<Texture_D3D9*>(h.getResPtr().get());
			HRESULT hr = mD3DEffect->SetTexture(paramHndl, tex->getD3DTexture());
			if (FAILED(hr)) {
				debugPrintf("Effect_D3D9: Error: failed to set texture \"%s\" in \"%s\"\n", resourcePath.c_str(), name().c_str());
				return false;
			}
			mResources.push_back(h.getResPtr());	// store a reference to the texture

		} else if (retVal == ResLoadResult_Error) {
			debugPrintf("Effect_D3D9: Error: failed to load child resource \"%s\" in \"%s\"\n", resourcePath.c_str(), name().c_str());
			return false;

		} else { // waiting
			// Submits a job that will run until the resource is available and loaded, or errors.
			// The job performs the same actions as above when the resource is loaded.
			CProcessPtr crProcPtr(new Effect_D3D9::CheckResourceLoadedProcess(this, paramHndl, resourcePath));
			procMgr.attach(crProcPtr);
			// add to the local list, the process won't be destroyed when finished, but this is neccesary
			// incase the effect is destroyed before the process finishes. In the destructor, the list is
			// traversed and all processes are killed
			mProcessList.push_back(crProcPtr);
		}

	} else {
		// for synchronous loading, false will be returned if any of the child
		// resources fails to load
		ResHandle h;
		if (h.load<Texture_D3D9>(resourcePath)) {
			Texture_D3D9 *tex = static_cast<Texture_D3D9*>(h.getResPtr().get());
			HRESULT hr = mD3DEffect->SetTexture(paramHndl, tex->getD3DTexture());
			if (FAILED(hr)) {
				debugPrintf("Effect_D3D9: Error: failed to set texture \"%s\" in \"%s\"\n", resourcePath.c_str(), name().c_str());
				return false;
			}
			mResources.push_back(h.getResPtr());	// store a reference to the texture

		} else {
			debugPrintf("Effect_D3D9: Error: failed to load child resource \"%s\" in \"%s\"\n", resourcePath.c_str(), name().c_str());
			return false;
		}
	}
	return true;
}

/*---------------------------------------------------------------------
---------------------------------------------------------------------*/
bool Effect_D3D11::loadScript(D3DXHANDLE paramHndl, const string &resourcePath)
{
	return true;
}

/*---------------------------------------------------------------------
	Called in a lost device situation for non-managed textures only.
	These will free and restore the resource.
---------------------------------------------------------------------*/
void Effect_D3D11::onDeviceLost()
{
	if (mInitialized) {
		mD3DEffect->OnLostDevice();
	}
}

void Effect_D3D11::onDeviceRestore()
{
	if (mInitialized) {
		mD3DEffect->OnResetDevice();
	}
}

/*---------------------------------------------------------------------
	releases the D3D9 effect resource and sets mInitialized false
---------------------------------------------------------------------*/
void Effect_D3D11::clearEffectData()
{
	if (mInitFlags[INIT_EFFECT]) { SAFE_RELEASE(mD3DEffect); }
	mResources.clear();
	mResourceCount = 0;
	mInitialized = false;
	// for each process in the list, if it's not already finished, kill it
	ProcessList::const_iterator i, end = mProcessList.end();
	for (i = mProcessList.begin(); i != end; ++i) {
		if (!(*i)->isFinished()) { (*i)->finish(); }
	}
}

// Constructors / destructor
/*---------------------------------------------------------------------
	constructor with this signature is required for the resource system
---------------------------------------------------------------------*/
Effect_D3D11::Effect_D3D11(const wstring &name, size_t sizeB, const ResCachePtr &resCachePtr) :
	Effect_Base(name, sizeB, resCachePtr),
	mD3DEffect(0),
	mhTechnique(0)
{
	SecureZeroMemory(&mMatrixHandle, sizeof(mMatrixHandle));
	SecureZeroMemory(&mVectorHandle, sizeof(mVectorHandle));
	SecureZeroMemory(&mScalarHandle, sizeof(mScalarHandle));
	SecureZeroMemory(&mTextureHandle, sizeof(mTextureHandle));
}

/*---------------------------------------------------------------------
	use this default constructor to create the texture without caching,
	or for cache injection method.
---------------------------------------------------------------------*/
Effect_D3D11::Effect_D3D11() :
	Effect_Base(),
	mD3DEffect(0),
	mhTechnique(0)
{
	SecureZeroMemory(&mMatrixHandle, sizeof(mMatrixHandle));
	SecureZeroMemory(&mVectorHandle, sizeof(mVectorHandle));
	SecureZeroMemory(&mScalarHandle, sizeof(mScalarHandle));
	SecureZeroMemory(&mTextureHandle, sizeof(mTextureHandle));
}

Effect_D3D11::~Effect_D3D11()
{
	clearEffectData();
}

////////// class CheckResourceLoadedProcess //////////

void Effect_D3D11::CheckResourceLoadedProcess::onUpdate(float deltaMillis) {
	ResHandle h;
	ResLoadResult retVal = h.tryLoad<Texture2DImpl>(mResourcePath);
	if (retVal == ResLoadResult_Success) {
		Texture2DImpl *tex = static_cast<Texture2DImpl*>(h.getResPtr().get());
		HRESULT hr = mpEffect->mD3DEffect->SetTexture(mParamHandle, tex->getD3DTexture());
		if (FAILED(hr)) {
			debugWPrintf(L"Effect_D3D11: Error: failed to set texture \"%s\" in \"%s\"\n", mResourcePath.c_str(), mpEffect->name().c_str());
			mpEffect->mInitialized = false; // set initialized back to false to be consistent with the way synchronous
											// loading leaves the effect state
		} else {
			mpEffect->mResources.push_back(h.getResPtr());	// store a reference to the texture
		}
		finish();

	} else if (retVal == ResLoadResult_Error) {
		debugWPrintf(L"Effect_D3D11: Error: failed to load child resource \"%s\" in \"%s\"\n", mResourcePath.c_str(), mpEffect->name().c_str());
		mpEffect->mInitialized = false;
		finish();
	}
}

Effect_D3D11::CheckResourceLoadedProcess::CheckResourceLoadedProcess(
		Effect_D3D11 *pEff, D3DXHANDLE paramHandle,
		const wstring &resourcePath) :
	Process("CheckResourceLoadedProcess", CProcess_Run_CanDelay, CProcess_Queue_Multiple),
	mpEffect(pEff),
	mResourcePath(resourcePath),
	mParamHandle(paramHandle)
{}
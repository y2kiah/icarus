/* Material.cpp
Author: Jeff Kiah
Orig.Date: 06/17/2012
*/

#include "Render/Material.h"
#include "Render/Texture2D.h"
#include "Process/ProcessManager.h"

/*---------------------------------------------------------------------
	Returns true if all child resources are loaded. Check before using
	the effect/material for rendering.
---------------------------------------------------------------------*/
bool Material::isValid() const
{
	if (m_textureCount == m_textures.size()/* && m_effect*/) {
		// also check if effect defaults and textures match effect requirements
		//Effect_D3D9 *pEff = (Effect_D3D9 *)mEffect.get();
		//return pEff->isValid();
		return true;
	}
	return false;
}

/*---------------------------------------------------------------------
	Adds a texture to the end of list. assumeCached true will assume
	that the texture is already in the ResCache_Material cache and will
	attempt to retrieve it. If false, async determines the method used
	to load it from a registered source. This function needs reference
	to a scheduler so it can attach load processes for child resources.
---------------------------------------------------------------------*/
bool Material::addTexture(const wstring &filename, uint32_t samplerIndex,
						  bool async, bool assumeCached, const SchedulerPtr &scheduler)
{
	++m_textureCount;
	// assumeCached is true when, for example, a texture is injected after loading it from disk,
	// or otherwise, anytime you're sure it's already present in the ResCache_Material cache
	if (assumeCached) {
		ResHandle h;
		if (!h.getFromCache(filename, ResCache_Material)) {
			debugWPrintf(L"Material::addTexture: Error: failed to load child resource \"%s\" from cache in material\n", filename.c_str());
			return false;
		}
		//mTextureFlags[mTextures.size()] = true;
		// store a reference to the texture
		m_textures.push_back(
			std::make_pair<Texture_NVP::first_type, Texture_NVP::second_type>(
				samplerIndex, h.getResPtr()
			)
		);
		return true;
	}
	// if assumeCached is false, load it from a registered source, where filename should
	// already contain the path of the source, e.g. "sourcename/filename.dds"
	if (async) {
		// for asynchronous loading, try to load the resource
		ResHandle h;
		ResLoadResult retVal = h.tryLoad<Texture2DImpl>(filename);
		if (retVal == ResLoadResult_Success) {
			//mTextureFlags[mTextures.size()] = true;
			// store a reference to the texture
			m_textures.push_back(
				std::make_pair<Texture_NVP::first_type, Texture_NVP::second_type>(
					samplerIndex, h.getResPtr()
				)
			);

		} else if (retVal == ResLoadResult_Error) {
			debugWPrintf(L"Material::addTexture: Error: failed to load child resource \"%s\" in material\n", filename.c_str());
			return false;

		} else { // waiting
			// Submits a job that will run until the resource is available and loaded, or errors.
			// The job performs the same actions as above when the resource is loaded.
			ProcessPtr procPtr(
				new Material::CheckResourceLoadedProcess(
								this,
								filename,
								samplerIndex,
								Material::CheckResourceLoadedProcess::MatResType_Texture)
			);

			if (!scheduler) {
				debugWPrintf(L"Material::addTexture: Error: failed to load child resource \"%s\" in material\n", filename.c_str());
				return false;
			}
			scheduler->attach(procPtr);

			m_processList.push_back(procPtr);
		}

	} else {
		// for synchronous loading, false will be returned if any of the child
		// resources fails to load
		ResHandle h;
		if (h.load<Texture2DImpl>(filename)) {
			//mTextureFlags[mTextures.size()] = true;
			// store a reference to the texture
			m_textures.push_back(
				std::make_pair<Texture_NVP::first_type, Texture_NVP::second_type>(
					samplerIndex, h.getResPtr()
				)
			);

		} else {
			debugWPrintf(L"Material::addTexture: Error: failed to load child resource \"%s\" in material\n", filename.c_str());
			return false;
		}
	}
	return true;
}

Material::~Material()
{
	// for each process in the list, if it's not already finished, kill it
	auto end = m_processList.end();
	for (auto i = m_processList.begin(); i != end; ++i) {
		if (!(*i)->isFinished()) { (*i)->finish(); }
	}
}

// class Material::CheckResourceLoadedProcess

void Material::CheckResourceLoadedProcess::onUpdate(double deltaMillis)
{
	ResHandle h;
	switch (m_resourceType) {
		// handle texture loading
		case MatResType_Texture: {
			ResLoadResult retVal = h.tryLoad<Texture2DImpl>(m_resourcePath);
			if (retVal == ResLoadResult_Success) {
				//m_pMaterial->mTextureFlags[mpMaterial->mTextures.size()] = true;
				// store a reference to the texture
				m_pMaterial->m_textures.push_back(
					std::make_pair<Texture_NVP::first_type, Texture_NVP::second_type>(
						m_samplerIndex, h.getResPtr()
					)
				);
				finish();

			} else if (retVal == ResLoadResult_Error) {
				debugWPrintf(L"Material: Error: failed to load child resource \"%s\" in material\n", m_resourcePath.c_str());
				finish();
			}
			break;
		}
		// handle effect loading
		case MatResType_Effect: {
			/*ResLoadResult retVal = h.tryLoad<EffectImpl>(mResourcePath);
			if (retVal == ResLoadResult_Success) {
				m_pMaterial->mEffect = h.getResPtr(); // store a reference to the texture
				finish();

			} else if (retVal == ResLoadResult_Error) {
				debugWPrintf(L"Material: Error: failed to load effect \"%s\" in material\n", m_resourcePath.c_str());
				finish();
			}*/
			break;
		}
	}
}

Material::CheckResourceLoadedProcess::CheckResourceLoadedProcess(
				Material *pMaterial, const wstring &resourcePath,
				uint32_t samplerIndex, MaterialResourceType resourceType) :
	Process("CheckResourceLoadedProcess", Process_Run_Frame, Process_Queue_Multiple),
	m_resourcePath(resourcePath),
	m_resourceType(resourceType),
	m_samplerIndex(samplerIndex),
	m_pMaterial(pMaterial)
{}
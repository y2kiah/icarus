/* Material.h
Author: Jeff Kiah
Orig.Date: 06/03/2012
*/
#pragma once

#include "Resource/ResHandle.h"
#include "Render/RenderBuffer.h"
#include "Math/Vector3f.h"
#include "Math/Matrix4x4f.h"

class Material {
	private:
		///// DEFINITIONS /////
		typedef std::pair<string, Vector3f>		Vector3_NVP;
		typedef std::pair<string, Matrix4x4f>	Matrix_NVP;
		typedef std::pair<uint32_t, ResPtr>		Texture_NVP;
		typedef std::pair<string, float>		Float_NVP;

		///// STRUCTURES /////
		/*---------------------------------------------------------------------
		class CheckResourceLoadedProcess
			A list of spawned processes is maintained in the material to
			protect against dereferencing a bad pointer in this process. If the
			material is destroyed first, it will call finish() on all processes
			in the list that aren't already finished.
		---------------------------------------------------------------------*/
		class CheckResourceLoadedProcess : public Process {
			public:
				enum MaterialResourceType : int {
					MatResType_Texture = 0,
					MatResType_Effect
				};
				
				virtual void onUpdate(double deltaMillis);
				virtual bool onInitialize() { return true; }
				virtual void onFinish() {}
				virtual void onTogglePause() {}

				explicit CheckResourceLoadedProcess(Material *pMaterial, const wstring &resourcePath,
													uint32_t samplerIndex, MaterialResourceType resourceType);
				virtual ~CheckResourceLoadedProcess() {}

			private:
				wstring					m_resourcePath;
				MaterialResourceType	m_resourceType;	// Texture or Effect
				uint32_t				m_samplerIndex; // 0-15
				Material *				m_pMaterial;
		};

		///// VARIABLES /////
		uint32_t			m_textureCount;	// number of textures to be loaded
		
		vector<Vector3_NVP>	m_vector3Data;	// colors and other vector data
		vector<Matrix_NVP>	m_matrixData;	// matrices
		vector<Texture_NVP>	m_textures;		// list of textures that have been loaded
		vector<Float_NVP>	m_scalarData;	// floats

		// reference to an Effect or Shader program

		RenderBufferUniquePtr m_constantBuffer;	// fill this with relevant data to feed to shader

		ProcessList			m_processList;		// list of processes spawned to check for loaded async resources

	public:
		///// FUNCTIONS /////
		/*---------------------------------------------------------------------
			Returns true if all child resources are loaded. Check before using
			the effect/material for rendering.
		---------------------------------------------------------------------*/
		bool isValid() const;

		/*---------------------------------------------------------------------
			Adds a texture to the end of list. Texture will be retrieved from
			the resource cache. Scheduler is only required if you choose to do
			async loading. See CPP file for more details.
		---------------------------------------------------------------------*/
		bool addTexture(const wstring &filename, uint32_t samplerIndex,
						bool async, bool assumeCached = false,
						const SchedulerPtr &scheduler = SchedulerPtr());

		// Constructors / destructor
		explicit Material() :
			m_textureCount(0), m_textures(8)
		{}

		~Material();
};
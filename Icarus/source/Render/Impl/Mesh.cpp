/* Mesh.cpp
Author: Jeff Kiah
Orig.Date: 06/17/2012
*/
#include "Render/Mesh.h"
#include "Render/Material.h"
#include "Render/Vertex_Defs.h"

// class Model

// I don't like this. passing a matrix to render() seems hackish.
// maybe instead a "RenderProperties" object can be injected which includes everything needed to render
// I think this function should submitting a render packet to the renderer which then optimizes for
// rendering order. Also, where does SceneLayer come into play?
void Model::render(const Matrix4x4f &world, const MeshNode &mNode, bool transparentPass)
{
	// Set vertex and index buffers
	//Program.device.VertexDeclaration = vertexDecl;
	//Program.device.SetStreamSource(0, vertexBuffer, 0, VertexPNTC.sizeInBytes);
	//Program.device.Indices = indexBuffer;

	// Draw each set
	//auto end = m_subsets.end();
	//for (auto i = m_subsets.begin(); i != end; ++i) {
		//MeshSubset &subset = *i;

		// skip the geometry if this is not the right pass, opaque vs. transparent
		/*if (drawSet.material->isTransparent() == transparentPass) {

			// Update the shader variables
			Program.effect.SetValue<Matrix>(Program.worldMatrixHandle, world);
			if (mat.isTextured) {
				Program.effect.SetValue<float>(Program.textureWeightHandle, 1);
				if (mat.texture[(int)Material.LightType.diffuse] != null) {
					Program.effect.SetTexture(Program.diffuseTextureHandle, mat.texture[(int)Material.LightType.diffuse]);
				}
				if (mat.texture[(int)Material.LightType.transparent] != null) {
					Program.effect.SetTexture(Program.transparentTextureHandle, mat.texture[(int)Material.LightType.transparent]);
				}

			} else { // no textures, color only
				Program.effect.SetValue<float>(Program.textureWeightHandle, 0);
				Program.effect.SetValue<Color4>(Program.diffuseHandle, mat.color[(int)Material.LightType.diffuse]);
				Program.effect.SetValue<Color4>(Program.transparentHandle, mat.color[(int)Material.LightType.transparent]);
			}
			Program.effect.CommitChanges();

			// Draw the set
			Program.device.DrawIndexedPrimitives(d.type, 0, 0, vertexCount, d.startIndex, d.primitiveCount);
		}*/
	//}
}

/*---------------------------------------------------------------------
	onLoad is called automatically by the resource caching system when
	a resource is first loaded from disk and added to the cache. This
	function loads the binary mesh file format.
---------------------------------------------------------------------*/
bool Model::onLoad(const CharBufferPtr &dataPtr, bool async)
{
	// if the resource is loaded using async method, onThreadInit will
	// be called automatically on a separate thread. If loading sync styley,
	// we need to call the routine to perform the same actions
	if (!async) {
		return onThreadInit(dataPtr);
	}
	//return true;
	return false; // until implemented
}

bool Model::onThreadInit(const CharBufferPtr &dataPtr)
{
	return false; // until implemented
}

#if defined(ICARUS_DEV_TOOLS)
#if defined(_DEBUG)
#pragma comment( lib, "assimp64d.lib" )
#else
#pragma comment( lib, "assimp64.lib" )
#endif

#include <assimp/Importer.hpp>	// C++ importer interface
#include <assimp/scene.h>		// Output data structure
#include <assimp/postprocess.h>	// Post processing flags
#include <assimp/config.h>		// Configuration properties

using namespace Assimp;

/*---------------------------------------------------------------------
	Import a 3d model using Assimp
---------------------------------------------------------------------*/
bool Model::importFromFile(const string &filename)
{
	Importer importer;
	importer.SetPropertyInteger(AI_CONFIG_PP_SBP_REMOVE, aiPrimitiveType_POINT | aiPrimitiveType_LINE);

	uint32_t ppFlags =	aiProcess_ConvertToLeftHanded |
						aiProcess_TransformUVCoords |
						aiProcessPreset_TargetRealtime_MaxQuality;

	const aiScene *scene = importer.ReadFile(filename, ppFlags);

	if (!scene) {
		debugPrintf("Mesh::importFromFile: failed: %s\n", importer.GetErrorString());
		return false;
	}

	// get Meshes
	m_meshes.reserve(scene->mNumMeshes);
	for (uint32_t m = 0; m < scene->mNumMeshes; ++m) {
		// only looking for triangles
		if (scene->mMeshes[m]->mPrimitiveTypes == aiPrimitiveType_TRIANGLE) {
			// create the Mesh
			MeshPtr meshPtr(new Mesh());

			// allocate vertex buffer
			unique_ptr<Vertex_PN[]> verts(new Vertex_PN[scene->mMeshes[m]->mNumVertices]);

			// for each vertex
			for (uint32_t v = 0; v < scene->mMeshes[m]->mNumVertices; ++v) {
				// copy the vertex
				verts[v].pos.assign(scene->mMeshes[m]->mVertices[v].x,
									scene->mMeshes[m]->mVertices[v].y,
									scene->mMeshes[m]->mVertices[v].z);

				verts[v].norm.assign(scene->mMeshes[m]->mNormals[v].x,
									 scene->mMeshes[m]->mNormals[v].y,
									 scene->mMeshes[m]->mNormals[v].z);
			}

			// allocate index buffer
			int numIndices = scene->mMeshes[m]->mNumFaces * 3;
			unique_ptr<int[]> indices(new int[numIndices]);

			// for each face
			for (uint32_t f = 0; f < scene->mMeshes[m]->mNumFaces; ++f) {
				_ASSERTE(scene->mMeshes[m]->mFaces[f].mNumIndices == 3 && "The face isn't 3 indices!");
				// copy the face indices
				indices[f*3]	= scene->mMeshes[m]->mFaces[f].mIndices[0];
				indices[f*3+1]	= scene->mMeshes[m]->mFaces[f].mIndices[1];
				indices[f*3+2]	= scene->mMeshes[m]->mFaces[f].mIndices[2];
			}

			// create the RenderBuffers to be passed to the Mesh
			RenderBufferUniquePtr vb(new RenderBufferImpl());
			if (!vb->createFromMemory(	RenderBuffer::VertexBuffer,
										verts.get(),
										sizeof(verts.get()),
										sizeof(Vertex_PN), 0))
			{
				debugPrintf("Mesh::importFromFile: failed to create vertex buffer: %s\n");
				return false;
			}

			RenderBufferUniquePtr ib(new RenderBufferImpl());
			if (!ib->createFromMemory(	RenderBuffer::IndexBuffer,
										indices.get(),
										sizeof(indices.get()),
										sizeof(int), 0))
			{
				debugPrintf("Mesh::importFromFile: failed to create index buffer: %s\n");
				return false;
			}

			// build the DrawSet for this mesh (only one needed)
			DrawSet ds;
			ds.startIndex = 0;
			ds.stopIndex = numIndices-1;
			ds.primitiveCount = scene->mMeshes[m]->mNumFaces;
			ds.type = TriangleList;
			ds.materialIndex = scene->mMeshes[m]->mMaterialIndex;

			// assemble the Mesh object and add it
			meshPtr->setVertexBuffer(vb);
			meshPtr->setIndexBuffer(ib);
			meshPtr->addDrawSet(ds);
			m_meshes.push_back(meshPtr);
		}
	}

	// get Materials
	m_materials.reserve(scene->mNumMaterials);
	for (uint32_t m = 0; m < scene->mNumMaterials; ++m) {
		MaterialPtr matPtr(new Material());
		m_materials.push_back(matPtr);
		
		// for each texture type
		uint32_t samplerIndex = 0;
		for (uint32_t tt = 0; tt < AI_TEXTURE_TYPE_MAX; ++tt) {
			// for each texture of a type
			for (uint32_t i = 0; i < scene->mMaterials[m]->GetTextureCount((aiTextureType)tt); ++i) {
				aiString path;
				scene->mMaterials[m]->GetTexture((aiTextureType)tt, i, &path); // get the name, ignore other attributes for now
				
				// convert from ascii to wide character set, may need to also add a prefix for resource location
				string aPath(path.data);
				wstring wPath;
				wPath.assign(aPath.begin(), aPath.end());

				// do synchronous loading since this is a utility function
				// if needed could also build the texture here and inject into cache, then pass assumeCached=true
				matPtr->addTexture(wPath, samplerIndex, false);

				++samplerIndex;
			}
		}
	}

	// get Scene graph
	std::function<void(aiNode*,MeshNode&)> traverseScene;
	traverseScene = [&traverseScene](aiNode *node, MeshNode &thisNode) {
		// set mesh indices
		for (uint32_t m = 0; m < node->mNumMeshes; ++m) {
			thisNode.m_meshIndex.push_back(node->mMeshes[m]);
		}
		// set child nodes
		for (uint32_t c = 0; c < node->mNumChildren; ++c) {
			// convert the transform matrix from aiMatrix4x4 to Matrix4x4f
			aiMatrix4x4 &t = node->mChildren[c]->mTransformation;
			const float pTransform[16] = { t.a1, t.a2, t.a3, t.a4, t.b1, t.b2, t.b3, t.b4, t.c1, t.c2, t.c3, t.c4, t.d1, t.d2, t.d3, t.d4 };
			Matrix4x4f transform(pTransform);
			// create and add the new node
			SceneNodePtr newNode(new MeshNode(transform,
											  thisNode.shared_from_this(),
											  node->mChildren[c]->mName.data));
			thisNode.addChild(newNode);
			// recursive call into child node
			traverseScene(node->mChildren[c], *(static_cast<MeshNode*>(newNode.get())));
		}
	};
	// start building scene at root
	traverseScene(scene->mRootNode, *(static_cast<MeshNode*>(m_root.get())));

	// set the model name
	mName.assign(filename.begin(), filename.end());

	// everything "assimp" is cleaned up by importer destructor
	return true;
}

/*---------------------------------------------------------------------
	Saves model to .imf (Icarus model format) file
---------------------------------------------------------------------*/
bool Model::saveToFile(const wstring &filename)
{
	return false; // until implemented
}

#endif
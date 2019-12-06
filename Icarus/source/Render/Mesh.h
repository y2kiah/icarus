/* Mesh.h
Author: Jeff Kiah
Orig.Date: 06/03/2012
*/
#pragma once

#include "Resource/ResHandle.h"
#include "RenderBuffer.h"
#include "Scene/Scene.h"

// forward declarations
class Material;
typedef shared_ptr<Material>	MaterialPtr;

enum PrimitiveType {
	TriangleList = 0, TriangleStrip,
	LineList, LineStrip,
	PointList, PointStrip
};

struct DrawSet {
	int				startIndex, stopIndex, primitiveCount;
	int				materialIndex; // index into the parent Model's m_materials array
	PrimitiveType	type;
};

class Mesh {
	private:
		///// VARIABLES /////
		// Member variables
		RenderBufferUniquePtr	m_vertexBuffer;
		RenderBufferUniquePtr	m_indexBuffer;
		vector<DrawSet>			m_drawSets;

	public:
		///// FUNCTIONS /////
		void setVertexBuffer(RenderBufferUniquePtr &vb)	{ m_vertexBuffer = std::move(vb); }
		void setIndexBuffer(RenderBufferUniquePtr &ib)	{ m_indexBuffer  = std::move(ib); }
		void addDrawSet(const DrawSet &ds)				{ m_drawSets.push_back(ds); }

		// Constructors / destructor
		explicit Mesh() {}
		~Mesh() {}
};

typedef shared_ptr<Mesh>	MeshPtr;

// still no virtual functions, maybe render() will be??
class MeshNode : public SceneNode {
	public:
		vector<int> m_meshIndex; // index into the parent Model's m_meshes array

		explicit MeshNode(const Matrix4x4f &object,
						  const SceneNodePtr &parent = SceneNodePtr(),
						  const string &name = "") :
			SceneNode(object, parent, name)
		{}
};

class Model : public Resource {
	private:
		///// VARIABLES /////
		// scene tree with node for each MeshInstance
		SceneNodePtr		m_root;

		vector<MeshPtr>		m_meshes;
		vector<MaterialPtr>	m_materials;

		// different model instances should have material overrides
		// if override is present would take place of Mesh default material
		
		// need animation controller

	public:
		///// FUNCTIONS /////
		// I don't like this. passing a matrix to render() seems hackish.
		// maybe instead a "RenderProperties" object can be injected which includes everything needed to render
		// I think this function should submitting a render packet to the renderer which then optimizes for
		// rendering order. Also, where does SceneLayer come into play?
		void render(const Matrix4x4f &world, const MeshNode &mNode, bool transparentPass);

		/*---------------------------------------------------------------------
			These virtual methods must be implemented for Resource system.
		---------------------------------------------------------------------*/
		virtual bool useThreadInit() const { return true; }
		virtual bool onLoad(const CharBufferPtr &dataPtr, bool async);
		virtual bool onThreadInit(const CharBufferPtr &dataPtr);

		// Misc functions
		#if defined(ICARUS_DEV_TOOLS)
		/*---------------------------------------------------------------------
			Import a 3d model using Assimp
		---------------------------------------------------------------------*/
		bool importFromFile(const string &filename);
		
		/*---------------------------------------------------------------------
			Saves model to .imf (Icarus model format) file
		---------------------------------------------------------------------*/
		bool saveToFile(const wstring &filename);
		#endif

		explicit Model() :
			Resource(),
			m_root(new MeshNode(Matrix4x4f::identity, SceneNodePtr(), "root"))
		{}
		explicit Model(const wstring &name, size_t sizeB, const ResCachePtr &resCachePtr) :
			Resource(name, sizeB, resCachePtr),
			m_root(new MeshNode(Matrix4x4f::identity, SceneNodePtr(), "root"))
		{}
		virtual ~Model() {}
};
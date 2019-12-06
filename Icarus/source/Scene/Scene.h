/* Scene.h
Author: Jeff Kiah
Orig.Date: 6/16/12
*/
#pragma once

#include <cstdint>
#include <list>
#include <vector>
#include <memory>
#include "Math/Matrix4x4f.h"

using std::list;
using std::vector;
using std::shared_ptr;
using std::weak_ptr;
using std::unique_ptr;

class SceneLayer;
class SceneNode;
typedef shared_ptr<SceneLayer>	SceneLayerPtr;
typedef list<SceneLayerPtr>		SceneLayerList;
typedef shared_ptr<SceneNode>	SceneNodePtr;
typedef weak_ptr<SceneNode>		SceneNodeWeakPtr;
typedef unique_ptr<SceneNode>	SceneNodeUniquePtr;
typedef vector<SceneNodePtr>	SceneNodeList;

class Scene {
	private:
		SceneLayerList	m_layers;

	public:
		// load scene
		// pre-load resources
		// render scene

		explicit Scene() {}
};

class SceneNode :
	public std::enable_shared_from_this<SceneNode>
{
	private:
		Matrix4x4f	m_localTransform;	// transformation applied in object space
		Matrix4x4f	m_objectOrig;		// the baseline object transform
		Matrix4x4f	m_world;			// to-world space transform
		bool		m_moved;			// set true to recalc world matrix
		string		m_name;				// name for logging and debugging
		
		SceneNodeWeakPtr	m_parent;	// pointer to parent node, null if this is tree root
		SceneNodeList		m_childNodes; // pointers to child nodes

	public:
		// Accessors
		inline const Matrix4x4f &getLocalTransform() const;
		inline const Matrix4x4f &getObjectOrigTransform() const;
		inline const Matrix4x4f &getToWorldTransform() const;
		inline bool moved() const;
		
		// Mutators
		inline void setLocalTransform(const Matrix4x4f &m);
		inline size_t addChild(const SceneNodePtr &n);

		// Functions
		/*---------------------------------------------------------------------
			used in traversal of scene graph, don't use for node translations
		---------------------------------------------------------------------*/
		inline void transform(Matrix4x4f toWorld, bool ancestorMoved);

		// Constructor
		inline explicit SceneNode(const Matrix4x4f &object,
								  const SceneNodePtr &parent = SceneNodePtr(),
								  const string &name = "");
};

class SceneLayer {
	protected:
		SceneNode		m_root;	// scene tree root node
		uint8_t			m_id;	// unique id for renderer bucket sorting
		string			m_name;	// name for logging and debugging

	public:
		// Accessors
		const uint8_t getId() const { return m_id; }
		const string &getName() const { return m_name; }

		// Mutators
		inline void transform();

		explicit SceneLayer(uint8_t id, const string &name) :
			m_root(Matrix4x4f::identity), m_id(id), m_name(name)
		{}
};

#include "Impl/Scene.inl"
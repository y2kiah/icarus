/* Scene.inl
Author: Jeff Kiah
Orig.Date: 6/16/12
*/
#pragma once

#include "Scene/Scene.h"
#include <algorithm>

using std::for_each;

// class SceneLayer

inline void SceneLayer::transform()
{
	// transform the scene tree starting at root
	m_root.transform(Matrix4x4f::identity, false);
}

// class SceneNode

inline const Matrix4x4f &SceneNode::getLocalTransform() const
{
	return m_localTransform;
}

inline const Matrix4x4f &SceneNode::getObjectOrigTransform() const
{
	return m_objectOrig;
}

inline const Matrix4x4f &SceneNode::getToWorldTransform() const
{
	return m_world;
}

inline bool SceneNode::moved() const
{
	return m_moved;
}

inline void SceneNode::setLocalTransform(const Matrix4x4f &m)
{
	m_localTransform = m;
	m_moved = true;
}

inline void SceneNode::transform(Matrix4x4f toWorld, bool ancestorMoved)
{
	// recalc world matrix if this, or any acestors have moved since last frame
	bool calcWorld = m_moved || ancestorMoved;
	if (calcWorld) {
		// may need to tweak the ordering of this multiply
		m_world = m_localTransform
					.multiply(m_objectOrig)
					.multiply(toWorld);
		m_moved = false;
	}

	// transform all child nodes
	for_each(m_childNodes.begin(), m_childNodes.end(), [&](const SceneNodePtr &node) {
		node->transform(m_world, calcWorld);
	});
}

inline size_t SceneNode::addChild(const SceneNodePtr &n)
{
	m_childNodes.push_back(n);
	return m_childNodes.size() - 1; // return index of child node
}

inline SceneNode::SceneNode(const Matrix4x4f &object,
							const SceneNodePtr &parent,
							const string &name) :
	m_objectOrig(object),
	m_moved(true),
	m_parent(parent),
	m_name(name)
{
	m_localTransform.setIdentity();
}
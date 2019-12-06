/* Camera.cpp
Author: Jeff Kiah
Orig.Date: 7/13/12
*/
#include "Scene/Camera.h"


void Camera::setViewTranslationPitchYawRoll(const Vector3f &pos, const Vector3f &viewAngles)
{
	m_pos = pos;
	m_viewAngles = viewAngles;
	m_rotationQuat.setRotationRollPitchYawV(viewAngles);
	
	m_translationMat.setTranslationV(m_pos);
	m_rotationMat.setRotationRollPitchYawV(viewAngles);

	m_viewMat.multiply(m_translationMat, m_rotationMat);

	setLocalTransform(m_translationMat); // update for SceneNode base class
}

void Camera::setViewTranslationLookTo(const Vector3f &pos, const Vector3f &dir, const Vector3f &up)
{
	m_pos = pos;

	// update the rest of the information!!
	m_translationMat.setTranslationV(m_pos);

	m_viewMat.setViewMatrixLH(pos, dir, up);

	setLocalTransform(m_translationMat); // update for SceneNode base class
}

void Camera::setViewTranslationQuaternion(const Vector3f &pos, const Quaternion &q)
{
	m_pos = pos;
	m_rotationQuat = q;
	// update view angles

	m_translationMat.setTranslationV(pos);
	m_rotationMat.setRotationQuaternion(q);

	m_viewMat.multiply(m_translationMat, m_rotationMat);
	
	setLocalTransform(m_translationMat); // update for SceneNode base class
}

void Camera::setProjectionAttributes(float fovX, float aspectRatio,
									 float zNear, float zFar, float zoom)
{
	m_fovX = fovX;
	m_fovY = fovX / aspectRatio;
	m_aspectRatio = aspectRatio;
	m_zNear = zNear;
	m_zFar = zFar;
	m_zoom = (zoom <= 0.0f) ? 1.0f : ((zoom >= 200.0f) ? 1.0f : zoom);

	m_projMat.setPerspectiveFovLH(m_fovY / m_zoom, m_aspectRatio, m_zNear, m_zFar);
}

Camera::Camera(const SceneNodePtr &parent, const string &name) :
	SceneNode(Matrix4x4f::identity, parent, name),
	m_fovX(0.0f), m_fovY(0.0f), m_aspectRatio(1.0f),
	m_zNear(0.0f), m_zFar(0.0f), m_zoom(1.0f)
{}


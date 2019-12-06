/* Camera.inl
Author: Jeff Kiah
Orig.Date: 7/13/12
*/
#pragma once

#include "Scene/Camera.h"

inline const Vector3f & Camera::getPos() const {
	return m_pos;
}

// returns axis angles in radians
inline const Vector3f & Camera::getViewAngles() const {
	return m_viewAngles;
}

// returns horizontal fov in radians
inline float Camera::getFovX() const	{
	return m_fovX;
}

// returns vertical fov in radians
inline float Camera::getFovY() const	{
	return m_fovY;
}
inline float Camera::getAspectRatio() const {
	return m_aspectRatio;
}

inline float Camera::getZNear() const {
	return m_zNear;
}

inline float Camera::getZFar() const {
	return m_zFar;
}

inline float Camera::getZoom() const {
	return m_zoom;
}

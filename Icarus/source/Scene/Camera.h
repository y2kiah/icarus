/* Camera.h
Author: Jeff Kiah
Orig.Date: 7/13/12
*/
#pragma once

#include "Scene.h"
#include "Math/Quaternion.h"

class Camera : public SceneNode {
	public:
		///// FUNCTIONS /////
		// Accessors
		inline const Vector3f & getPos() const;
		inline const Vector3f & getViewAngles() const;
		inline float getFovX() const;
		inline float getFovY() const;
		inline float getAspectRatio() const;
		inline float getZNear() const;
		inline float getZFar() const;
		inline float getZoom() const;

		// Mutators
		void setViewTranslationPitchYawRoll(const Vector3f &pos, const Vector3f &viewAngles);
		void setViewTranslationLookTo(const Vector3f &pos, const Vector3f &dir, const Vector3f &up = Vector3f(0.0f, 1.0f, 0.0f));
		void setViewTranslationQuaternion(const Vector3f &pos, const Quaternion &q);

		void setProjectionAttributes(float fovXRad, float aspectRatio, float zNear, float zFar, float zoom);
		
		void recomputeMatrices();

		// Constructor
		explicit Camera(const SceneNodePtr &parent = SceneNodePtr(),
						const string &name = "");

	private:
		///// VARIABLES /////
		Matrix4x4f	m_viewMat;		// view matrix

		Quaternion	m_rotationQuat;	// quaternion representing rotation
		Vector3f	m_viewAngles;	// angles representing rotation. Order: pitch, yaw, roll
		Matrix4x4f	m_rotationMat;	// rotation matrix

		Vector3f	m_pos;			// camera position
		Matrix4x4f	m_translationMat; // translation matrix
		
		Matrix4x4f	m_projMat;		// projection matrix
		float		m_fovX, m_fovY, m_aspectRatio; // perspective projection params
		float		m_zNear, m_zFar, m_zoom;
};

#include "Impl/Camera.inl"
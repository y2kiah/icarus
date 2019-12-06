/* Matrix4x4f.h
Author: Jeff Kiah
Orig.Date: 5/21/12
*/
#pragma once

#if defined(WIN32)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN	// defined in project settings
#endif
#include <windows.h>
#include <xnamath.h>
#endif

class Vector3f;
class Vector4f;
class Quaternion;

// class Matrix4x4f

class Matrix4x4f
#if defined(WIN32)
	: public XMFLOAT4X4 
#endif
{
	public:
		// Variables
		#if !defined(WIN32)
		union {
			struct {
				float _11,_12,_13,_14,_21,_22,_23,_24,
					  _31,_32,_33,_34,_41,_42,_43,_44;
			};
			float p[16];
			float m[4][4];
		};
		#endif

		static Matrix4x4f identity;

		// Operators

		inline void	operator= (const Matrix4x4f &m);
		inline void	operator*=(const Matrix4x4f &m);

		// Functions

		// Matrix operations
		inline void	multiply(const Matrix4x4f &m1, const Matrix4x4f &m2);
		inline void	multiplyTranspose(const Matrix4x4f &m1, const Matrix4x4f &m2);
		inline void	transposeOf(const Matrix4x4f &m);

		inline Matrix4x4f &	multiply(const Matrix4x4f &m);
		inline Matrix4x4f &	multiplyTranspose(const Matrix4x4f &m);
		inline Matrix4x4f &	transpose();

		// Matrix transforms
		inline void	set(const Matrix4x4f &m);
		inline void	setIdentity();
		// Translation
		inline void	setTranslation(float x, float y, float z);
		inline void setTranslationV(const Vector3f &v);
		// Scaling
		inline void	setScaling(float x, float y, float z);
		inline void setScalingV(const Vector3f &v);
		// Rotation
		inline void setRotationAxis(const Vector3f &axis, float angle);
		inline void setRotationNormal(const Vector3f &normAxis, float angle);
		inline void setRotationQuaternion(const Quaternion &q);
		inline void setRotationRollPitchYaw(float p, float y, float r);
		inline void setRotationRollPitchYawV(const Vector3f &v);
		inline void	setRotateX(float a);
		inline void	setRotateY(float a);
		inline void	setRotateZ(float a);
		// Perspective Projection
		inline void	setPerspectiveFovLH(float fovY, float aspectRatio, float zNear, float zFar);
		inline void	setPerspectiveFovRH(float fovY, float aspectRatio, float zNear, float zFar);
		inline void	setPerspectiveLH(float nearWidth, float nearHeight, float zNear, float zFar);
		inline void	setPerspectiveRH(float nearWidth, float nearHeight, float zNear, float zFar);
		// Orthographic Projection
		inline void	setOrthographicLH(float nearWidth, float nearHeight, float zNear, float zFar);
		inline void	setOrthographicRH(float nearWidth, float nearHeight, float zNear, float zFar);
		// Look-at View
		inline void	setLookAtLH(const Vector3f &eyePos, const Vector3f &lookAt, const Vector3f &upDir);
		inline void	setLookAtRH(const Vector3f &eyePos, const Vector3f &lookAt, const Vector3f &upDir);
		// Look-to View
		inline void	setViewMatrixLH(const Vector3f &eyePos, const Vector3f &eyeDir, const Vector3f &upDir);
		inline void	setViewMatrixRH(const Vector3f &eyePos, const Vector3f &eyeDir, const Vector3f &upDir);
		// Shadow
		//inline void setShadow(const Planef &shadowPlane, const Vector3f &lightPos);

		// Constructors / Destructor
		inline Matrix4x4f();
		inline Matrix4x4f(const float _p[16]);
		inline Matrix4x4f(const Matrix4x4f &m);
};

#if defined(WIN32)
#include "Impl\Matrix4x4f_xna.inl"
#endif
/* Matrix4x4f_xna.inl
Author: Jeff Kiah
Orig.Date: 5/21/12
*/
#pragma once

#include "Math/Matrix4x4f.h"
#include "Math/Vector3f.h"
#include "Math/Vector4f.h"
#include "Math/Quaternion.h"

// Operators

inline void Matrix4x4f::operator= (const Matrix4x4f &m)
{
	XMFLOAT4X4::operator=(m);
}

inline void Matrix4x4f::operator*=(const Matrix4x4f &m)
{
	multiply(*this, m);
}

// Functions

inline void Matrix4x4f::multiply(const Matrix4x4f &m1, const Matrix4x4f &m2)
{
	XMStoreFloat4x4(this,
					XMMatrixMultiply(XMLoadFloat4x4(&m1), XMLoadFloat4x4(&m2)));
}

inline void Matrix4x4f::multiplyTranspose(const Matrix4x4f &m1, const Matrix4x4f &m2)
{
	XMStoreFloat4x4(this,
					XMMatrixMultiplyTranspose(XMLoadFloat4x4(&m1), XMLoadFloat4x4(&m2)));
}

inline void	Matrix4x4f::transposeOf(const Matrix4x4f &m)
{
	XMStoreFloat4x4(this,
					XMMatrixTranspose(XMLoadFloat4x4(&m)));
}

inline Matrix4x4f &	Matrix4x4f::multiply(const Matrix4x4f &m)
{
	multiply(*this, m);
	return *this;
}

inline Matrix4x4f &	Matrix4x4f::multiplyTranspose(const Matrix4x4f &m)
{
	multiplyTranspose(*this, m);
	return *this;
}

inline Matrix4x4f &	Matrix4x4f::transpose()
{
	transposeOf(*this);
	return *this;
}

inline void Matrix4x4f::set(const Matrix4x4f &m)
{
	XMFLOAT4X4::operator=(m);
}

// Set matrix to the identity matrix
inline void Matrix4x4f::setIdentity()
{
	XMStoreFloat4x4(this, XMMatrixIdentity());
}

// Set matrix to a translation matrix
inline void Matrix4x4f::setTranslation(float x, float y, float z)
{
	XMStoreFloat4x4(this,
					XMMatrixTranslation(x, y, z));
}

inline void Matrix4x4f::setTranslationV(const Vector3f &v)
{
	XMStoreFloat4x4(this,
					XMMatrixTranslationFromVector(XMLoadFloat3(&v)));
}

// Set matrix to a scaling matrix
inline void Matrix4x4f::setScaling(float x, float y, float z)
{
	XMStoreFloat4x4(this,
					XMMatrixScaling(x, y, z));
}

inline void Matrix4x4f::setScalingV(const Vector3f &v)
{
	XMStoreFloat4x4(this,
					XMMatrixScalingFromVector(XMLoadFloat3(&v)));
}

inline void Matrix4x4f::setRotationAxis(const Vector3f &axis, float angle)
{
	XMStoreFloat4x4(this,
					XMMatrixRotationAxis(XMLoadFloat3(&axis), angle));
}

inline void Matrix4x4f::setRotationNormal(const Vector3f &normAxis, float angle)
{
	XMStoreFloat4x4(this,
					XMMatrixRotationNormal(XMLoadFloat3(&normAxis), angle));
}

inline void Matrix4x4f::setRotationQuaternion(const Quaternion &q)
{
	XMStoreFloat4x4(this,
					XMMatrixRotationQuaternion(XMLoadFloat4(&q)));
}

inline void Matrix4x4f::setRotationRollPitchYaw(float p, float y, float r)
{
	XMStoreFloat4x4(this,
					XMMatrixRotationRollPitchYaw(p, y, r));
}

inline void Matrix4x4f::setRotationRollPitchYawV(const Vector3f &v)
{
	XMStoreFloat4x4(this,
					XMMatrixRotationRollPitchYawFromVector(XMLoadFloat3(&v)));
}

inline void Matrix4x4f::setRotateX(float a)
{
	XMStoreFloat4x4(this, XMMatrixRotationX(a));
}

inline void Matrix4x4f::setRotateY(float a)
{
	XMStoreFloat4x4(this, XMMatrixRotationY(a));
}

inline void Matrix4x4f::setRotateZ(float a)
{
	XMStoreFloat4x4(this, XMMatrixRotationZ(a));
}

/*---------------------------------------------------------------------
xScale	    0           0           0
0         yScale        0           0
0           0        zf/(zf-zn)     1
0           0       -zn*zf/(zf-zn)  0
where:
yScale = cot(fovY/2)
xScale = yScale / aspect ratio
---------------------------------------------------------------------*/
inline void	Matrix4x4f::setPerspectiveFovLH(float fovY, float aspectRatio, float zNear, float zFar)
{
	XMStoreFloat4x4(this,
					XMMatrixPerspectiveFovLH(fovY, aspectRatio, zNear, zFar));
}

/*---------------------------------------------------------------------
xScale      0           0           0
0         yScale        0           0
0           0        zf/(zn-zf)    -1
0           0        zn*zf/(zn-zf)  0
where:
yScale = cot(fovY/2)
xScale = yScale / aspect ratio
---------------------------------------------------------------------*/
inline void	Matrix4x4f::setPerspectiveFovRH(float fovY, float aspectRatio, float zNear, float zFar)
{
	XMStoreFloat4x4(this,
					XMMatrixPerspectiveFovRH(fovY, aspectRatio, zNear, zFar));
}

/*---------------------------------------------------------------------
2*zn/w      0           0           0
0        2*zn/h         0           0
0           0        zf/(zf-zn)     1
0           0       -zn*zf/(zf-zn)  0
---------------------------------------------------------------------*/
inline void	Matrix4x4f::setPerspectiveLH(float nearWidth, float nearHeight, float zNear, float zFar)
{
	XMStoreFloat4x4(this,
					XMMatrixPerspectiveLH(nearWidth, nearHeight, zNear, zFar));
}

/*---------------------------------------------------------------------
2*zn/w      0           0           0
0        2*zn/h         0           0
0           0        zf/(zn-zf)    -1
0           0        zn*zf/(zn-zf)  0
---------------------------------------------------------------------*/
inline void	Matrix4x4f::setPerspectiveRH(float nearWidth, float nearHeight, float zNear, float zFar)
{
	XMStoreFloat4x4(this,
					XMMatrixPerspectiveRH(nearWidth, nearHeight, zNear, zFar));
}

inline void	Matrix4x4f::setOrthographicLH(float nearWidth, float nearHeight, float zNear, float zFar)
{
	XMStoreFloat4x4(this,
					XMMatrixOrthographicLH(nearWidth, nearHeight, zNear, zFar));
}

inline void	Matrix4x4f::setOrthographicRH(float nearWidth, float nearHeight, float zNear, float zFar)
{
	XMStoreFloat4x4(this,
					XMMatrixOrthographicRH(nearWidth, nearHeight, zNear, zFar));
}

inline void	Matrix4x4f::setLookAtLH(const Vector3f &eyePos, const Vector3f &lookAt, const Vector3f &upDir)
{
	XMStoreFloat4x4(this,
					XMMatrixLookAtLH(XMLoadFloat3(&eyePos), XMLoadFloat3(&lookAt), XMLoadFloat3(&upDir)));
}

inline void	Matrix4x4f::setLookAtRH(const Vector3f &eyePos, const Vector3f &lookAt, const Vector3f &upDir)
{
	XMStoreFloat4x4(this,
					XMMatrixLookAtRH(XMLoadFloat3(&eyePos), XMLoadFloat3(&lookAt), XMLoadFloat3(&upDir)));
}

inline void	Matrix4x4f::setViewMatrixLH(const Vector3f &eyePos, const Vector3f &eyeDir, const Vector3f &upDir)
{
	XMStoreFloat4x4(this,
					XMMatrixLookToLH(XMLoadFloat3(&eyePos), XMLoadFloat3(&eyeDir), XMLoadFloat3(&upDir)));
}

inline void	Matrix4x4f::setViewMatrixRH(const Vector3f &eyePos, const Vector3f &eyeDir, const Vector3f &upDir)
{
	XMStoreFloat4x4(this,
					XMMatrixLookToRH(XMLoadFloat3(&eyePos), XMLoadFloat3(&eyeDir), XMLoadFloat3(&upDir)));
}

// Constructors
inline Matrix4x4f::Matrix4x4f(const float _p[16]) :
	XMFLOAT4X4(_p)
{}

inline Matrix4x4f::Matrix4x4f()
{
	setIdentity();
}

inline Matrix4x4f::Matrix4x4f(const Matrix4x4f &m)
{
	set(m);
}
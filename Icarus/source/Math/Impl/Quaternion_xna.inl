/* Quaternion_xna.inl
Author:	Jeff Kiah
Orig.Date: 5/21/12
*/
#pragma once

#include "Math/Quaternion.h"
#include "Math/Matrix4x4f.h"
#include <sstream>

using std::ostringstream;

// Operators

inline Quaternion Quaternion::operator* (const Quaternion &q) const
{
	Quaternion t(*this);
	return t.multiply(q);
}

inline Quaternion Quaternion::operator* (const float s) const
{
	Quaternion t(*this);
	return t.scale(s);
}

// Functions

inline void Quaternion::setIdentity()
{
	XMStoreFloat4(this, XMQuaternionIdentity());
}

inline void Quaternion::setConjugate()
{
	XMStoreFloat4(	this,
					XMQuaternionConjugate(XMLoadFloat4(this)));
}

inline void Quaternion::setInverse()
{
	XMStoreFloat4(	this,
					XMQuaternionInverse(XMLoadFloat4(this)));
}

inline Quaternion & Quaternion::scale(const float s)
{
	scale(*this, s);
	return *this;
}

inline Quaternion & Quaternion::multiply(const Quaternion &q)
{
	multiply(*this, q);
	return *this;
}

inline void Quaternion::scale(const Quaternion &q, const float s)
{
	Vector4f::scale(s);
}

inline void Quaternion::multiply(const Quaternion &q0, const Quaternion &q1)
{
	XMStoreFloat4(	this,
					XMQuaternionMultiply(XMLoadFloat4(&q0), XMLoadFloat4(&q1)));
}

inline Quaternion & Quaternion::nlerp(const Quaternion &q0, const Quaternion &q1, const float t)
{
	// normalize(q0 + (q1 - q0)*t)
	subtract(q1, q0);
	scale(t);
	add(q0);
	normalize();
	return *this;
}

inline Quaternion & Quaternion::slerp(const Quaternion &q0, const Quaternion &q1, const float t)
{
	XMStoreFloat4(	this,
					XMQuaternionSlerp(XMLoadFloat4(&q0), XMLoadFloat4(&q1), t));
	return *this;
}

// Interpolates between four unit quaternions, using spherical quadrangle interpolation
inline Quaternion & Quaternion::squad(const Quaternion &q0, const Quaternion &q1,
									  const Quaternion &q2, const Quaternion &q3, const float t)
{
	// slerp(slerp(q0, c, t), slerp(a, b, t), 2t(1 - t))
	XMVECTOR q(XMLoadFloat4(&q0));
	XMVECTOR a, b, c;
	// this setup should probably be split out into another function to cache the a, b, c values between frames
	XMQuaternionSquadSetup(&a, &b, &c, q, XMLoadFloat4(&q1), XMLoadFloat4(&q2), XMLoadFloat4(&q3));

	XMStoreFloat4(	this,
					XMQuaternionSquad(q, a, b, c, t));
	return *this;
}

inline Quaternion & Quaternion::setRotationAxis(const Vector3f &axis, const float angle)
{
	XMStoreFloat4(	this,
					XMQuaternionRotationAxis(XMLoadFloat3(&axis), angle));
	return *this;
}

inline Quaternion & Quaternion::setRotationNormal(const Vector3f &normAxis, const float angle)
{
	XMStoreFloat4(	this,
					XMQuaternionRotationNormal(XMLoadFloat3(&normAxis), angle));
	return *this;
}

inline Quaternion & Quaternion::setRotationFromMatrix(const Matrix4x4f &m)
{
	XMStoreFloat4(	this,
					XMQuaternionRotationMatrix(XMLoadFloat4x4(&m)));
	return *this;
}

inline Quaternion & Quaternion::setRotationRollPitchYaw(float p, float y, float r)
{
	XMStoreFloat4(	this,
					XMQuaternionRotationRollPitchYaw(p, y, r));
	return *this;
}

inline Quaternion & Quaternion::setRotationRollPitchYawV(const Vector3f &v)
{
	XMStoreFloat4(	this,
					XMQuaternionRotationRollPitchYawFromVector(XMLoadFloat3(&v)));
	return *this;
}

inline void Quaternion::toAxisAngle(Vector3f &axis, float &angle) const
{
	XMVECTOR v;
	XMQuaternionToAxisAngle(&v, &angle, XMLoadFloat4(this));
	XMStoreFloat3(&axis, v);
}

// Constructors

inline Quaternion::Quaternion() :
	Vector4f(0, 0, 0, 0)
{}

// copy constructor
inline Quaternion::Quaternion(const Quaternion &q) :
	Vector4f(q.x, q.y, q.z, q.w)
{}

// construct quaternion from real component w and imaginary x,y,z
inline Quaternion::Quaternion(	const float _x, const float _y,
								const float _z, const float _w) :
	Vector4f(_x, _y, _z, _w)
{}

// construct quaternion from angle-axis
inline Quaternion::Quaternion(const Vector3f &axis, const float angle) :
	Vector4f()
{
	setRotationAxis(axis, angle);
}
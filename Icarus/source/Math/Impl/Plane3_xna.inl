/* Plane3_xna.inl
Author: Jeff Kiah
Orig.Date: 5/21/12
*/
#pragma once;

#include "Math/Plane3.h"
#include "Math/Matrix4x4f.h"

// Operators

inline bool Plane3::operator==(const Plane3 &p) const
{
	return (c == p.c);
}

inline bool Plane3::operator!=(const Plane3 &p) const
{
	return (c != p.c);
}

// Functions

inline float Plane3::findDist(const Vector3f &p) const
{
	return n.dot(p) - d;
}

inline void Plane3::set(const Vector3f &p1, const Vector3f &p2, const Vector3f &p3)
{
	XMStoreFloat4(&c,
				  XMPlaneFromPoints(XMLoadFloat3(&p1), XMLoadFloat3(&p2), XMLoadFloat3(&p3)));
}

inline void Plane3::set(const Vector3f &p, const Vector3f &unitNormal)
{
	XMStoreFloat4(&c,
				  XMPlaneFromPointNormal(XMLoadFloat3(&p), XMLoadFloat3(&unitNormal)));
}

// transform plane by given Matrix
inline void Plane3::transform(const Plane3 &p, const Matrix4x4f &m)
{
	XMStoreFloat4(&c,
				  XMPlaneTransform(XMLoadFloat4(&p.c), XMLoadFloat4x4(&m)));
}

inline Plane3 & Plane3::transform(const Matrix4x4f &m)
{
	transform(*this, m);
	return *this;
}

// Constructors
inline Plane3::Plane3() :
	c()
{}

inline Plane3::Plane3(const Vector3f &p1, const Vector3f &p2, const Vector3f &p3)
{
	set(p1, p2, p3);
}

inline Plane3::Plane3(const Vector3f &p, const Vector3f &unitNormal)
{
	set(p, unitNormal);
}

inline Plane3::Plane3(const Vector3f &unitNormal, float _d) :
	n(unitNormal), d(_d)
{}

inline Plane3::Plane3(const Vector4f &_c) :
	c(_c)
{}

inline Plane3::Plane3(const Plane3 &p) :
	c(p.c)
{}

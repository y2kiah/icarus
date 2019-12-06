/* Vector3f_xna.h
Author: Jeff Kiah
Orig.Date: 5/20/12
*/
#pragma once

#include "Math/Vector3f.h"
#include <sstream>

using std::ostringstream;

// Operators

inline bool Vector3f::operator==(const Vector3f &p) const
{
	return (XMVector3Equal(XMLoadFloat3(this), XMLoadFloat3(&p)) != FALSE);
}

inline bool Vector3f::operator!=(const Vector3f &p) const
{
	return (XMVector3NotEqual(XMLoadFloat3(this), XMLoadFloat3(&p)) != FALSE);
}

inline void Vector3f::operator= (const Vector3f &p) { assign(p); }
inline void Vector3f::operator+=(const Vector3f &p) { add(p); }
inline void Vector3f::operator-=(const Vector3f &p) { subtract(p); }
inline void Vector3f::operator*=(const Vector3f &p)	{ multiply(p); }
inline void Vector3f::operator/=(const Vector3f &p)	{ divide(p); }
inline void Vector3f::operator+=(const float s)		{ add(s); }
inline void Vector3f::operator-=(const float s)		{ subtract(s); }
inline void Vector3f::operator*=(const float s)		{ scale(s); }
inline void Vector3f::operator/=(const float s)		{ divide(s); }

inline Vector3f Vector3f::operator- () const
{
	return Vector3f(-x, -y, -z);
}

inline Vector3f Vector3f::operator+ (const float s) const
{
	Vector3f v(*this);
	return v.add(s);
}

inline Vector3f Vector3f::operator- (const float s) const
{
	Vector3f v(*this);
	return v.subtract(s);
}

inline Vector3f Vector3f::operator* (const float s) const
{
	Vector3f v(*this);
	return v.scale(s);
}

inline Vector3f Vector3f::operator/ (const float s) const
{
	Vector3f v(*this);
	return v.divide(s);
}

inline Vector3f Vector3f::operator+ (const Vector3f &p) const
{
	Vector3f v(*this);
	return v.add(p);
}

inline Vector3f Vector3f::operator- (const Vector3f &p) const
{
	Vector3f v(*this);
	return v.subtract(p);
}

inline Vector3f Vector3f::operator* (const Vector3f &p) const
{
	Vector3f v(*this);
	return v.multiply(p);
}

inline Vector3f Vector3f::operator/ (const Vector3f &p) const
{
	Vector3f v(*this);
	return v.divide(p);
}

// Functions

inline void Vector3f::assign(const float _x, const float _y, const float _z)
{
	x = _x; y = _y; z = _z;
}

inline void Vector3f::assign(const Vector3f &p)
{
	x = p.x; y = p.y; z = p.z;
}

inline bool Vector3f::nearEqualTo(const Vector3f &p) const
{
	return (XMVector3NearEqual(XMLoadFloat3(this), XMLoadFloat3(&p), XMVectorSplatEpsilon()) != FALSE);
}

inline void Vector3f::add(const Vector3f &p1,const Vector3f &p2)
{
	XMStoreFloat3(	this,
					XMVectorAdd(XMLoadFloat3(&p1), XMLoadFloat3(&p2)));
}

inline Vector3f & Vector3f::add(const Vector3f &p)
{
	add(*this, p);
	return *this;
}

inline void Vector3f::subtract(const Vector3f &p1,const Vector3f &p2)
{
	XMStoreFloat3(	this,
					XMVectorSubtract(XMLoadFloat3(&p1), XMLoadFloat3(&p2)));
}

inline Vector3f & Vector3f::subtract(const Vector3f &p)
{
	subtract(*this, p);
	return *this;
}

inline void Vector3f::multiply(const Vector3f &p1, const Vector3f &p2)
{
	XMStoreFloat3(	this,
					XMVectorMultiply(XMLoadFloat3(&p1), XMLoadFloat3(&p2)));
}

inline Vector3f & Vector3f::multiply(const Vector3f &p)
{
	multiply(*this, p);
	return *this;
}

inline void Vector3f::divide(const Vector3f &p1, const Vector3f &p2)
{
	XMStoreFloat3(	this,
					XMVectorDivide(XMLoadFloat3(&p1), XMLoadFloat3(&p2)));
}

inline Vector3f & Vector3f::divide(const Vector3f &p) {
	divide(*this, p);
	return *this;
}

inline void Vector3f::cross(const Vector3f &p1, const Vector3f &p2)
{
	XMStoreFloat3(	this,
					XMVector3Cross(XMLoadFloat3(&p1), XMLoadFloat3(&p2)));
}

inline Vector3f & Vector3f::cross(const Vector3f &p)
{
	cross(*this, p);
	return *this;
}

inline void Vector3f::lerp(const Vector3f &p1, const Vector3f &p2, const float t)
{
	XMStoreFloat3(	this,
					XMVectorLerp(XMLoadFloat3(&p1), XMLoadFloat3(&p2), t));
}

inline void Vector3f::lerpV(const Vector3f &p1, const Vector3f &p2, const Vector3f &t)
{
	XMStoreFloat3(	this,
					XMVectorLerpV(XMLoadFloat3(&p1), XMLoadFloat3(&p2), XMLoadFloat3(&t)));
}

inline Vector3f & Vector3f::lerp(const Vector3f &p, const float t)
{
	lerp(*this, p, t);
	return *this;
}

inline Vector3f & Vector3f::lerpV(const Vector3f &p, const Vector3f &t)
{
	lerpV(*this, p, t);
	return *this;
}

inline void Vector3f::unitNormalOf(const Vector3f &p1, const Vector3f &p2)
{
	cross(p1, p2);	// Calculates the normal vector with cross product
	normalize();	// Normalizes the vector
}

inline Vector3f & Vector3f::normalize()
{
	XMStoreFloat3(	this,
					XMVector3Normalize(XMLoadFloat3(this)));
	return *this;
}

inline Vector3f & Vector3f::normalizeEst()
{
	XMStoreFloat3(	this,
					XMVector3NormalizeEst(XMLoadFloat3(this)));
	return *this;
}

// Reflects an incident 3D vector across a 3D normal vector
inline void Vector3f::reflect(const Vector3f &incident, const Vector3f &normal)
{
	XMStoreFloat3(	this,
					XMVector3Reflect(XMLoadFloat3(&incident), XMLoadFloat3(&normal)));
}

inline Vector3f & Vector3f::reflect(const Vector3f &normal)
{
	reflect(*this, normal);
	return *this;
}

// Refracts an incident 3D vector across a 3D normal vector
inline void Vector3f::refract(	const Vector3f &incident, const Vector3f &normal,
								const float refractionIndex)
{
	XMStoreFloat3(	this,
					XMVector3Refract(XMLoadFloat3(&incident), XMLoadFloat3(&normal), refractionIndex));
}

inline Vector3f & Vector3f::refract(const Vector3f &normal, const float refractionIndex)
{
	refract(*this, normal, refractionIndex);
	return *this;
}

inline void Vector3f::add(const Vector3f &p, const float s)
{
	XMStoreFloat3(	this,
					XMVectorAdd(XMLoadFloat3(&p), XMVectorReplicate(s)));
}

inline Vector3f & Vector3f::add(const float s)
{
	add(*this, s);
	return *this;
}

inline void Vector3f::subtract(const Vector3f &p, const float s)
{
	XMStoreFloat3(	this,
					XMVectorSubtract(XMLoadFloat3(&p), XMVectorReplicate(s)));
}

inline Vector3f & Vector3f::subtract(const float s)
{
	subtract(*this, s);
	return *this;
}

inline void Vector3f::scale(const Vector3f &p, const float s)
{
	XMStoreFloat3(	this,
					XMVectorScale(XMLoadFloat3(&p), s));
}

inline Vector3f & Vector3f::scale(const float s)
{
	scale(*this, s);
	return *this;
}

inline void Vector3f::divide(const Vector3f &p, const float s)
{
	XMStoreFloat3(	this,
					XMVectorDivide(XMLoadFloat3(&p), XMVectorReplicate(s)));
}

inline Vector3f & Vector3f::divide(float s)
{
	divide(*this, s);
	return *this;
}

inline float Vector3f::dot(const Vector3f &p) const
{
	return XMVectorGetX(XMVector3Dot(XMLoadFloat3(this), XMLoadFloat3(&p)));
}

inline float Vector3f::distanceTo(const Vector3f &p) const
{
	XMVECTOR v(XMVectorSubtract(XMLoadFloat3(this), XMLoadFloat3(&p)));
	return XMVectorGetX(XMVector3Length(v));
}

inline float Vector3f::distanceToEst(const Vector3f &p) const
{
	XMVECTOR v(XMVectorSubtract(XMLoadFloat3(this), XMLoadFloat3(&p)));
	return XMVectorGetX(XMVector3LengthEst(v));
}

inline float Vector3f::distSquaredTo(const Vector3f &p) const
{
	XMVECTOR v(XMVectorSubtract(XMLoadFloat3(this), XMLoadFloat3(&p)));
	return XMVectorGetX(XMVector3LengthSq(v));
}

// Computes the minimum distance to a line
inline float Vector3f::linePointDistance(const Vector3f &lp1, const Vector3f &lp2)
{
	return XMVectorGetX(XMVector3LinePointDistance(
							XMLoadFloat3(&lp1), XMLoadFloat3(&lp2), XMLoadFloat3(this)));
}

inline float Vector3f::length() const
{
	return XMVectorGetX(XMVector3Length(XMLoadFloat3(this)));
}

inline float Vector3f::lengthEst() const
{
	return XMVectorGetX(XMVector3LengthEst(XMLoadFloat3(this)));
}

inline float Vector3f::lengthSquared() const
{
	return XMVectorGetX(XMVector3LengthSq(XMLoadFloat3(this)));
}

inline float Vector3f::recipLength() const
{
	return XMVectorGetX(XMVector3ReciprocalLength(XMLoadFloat3(this)));
}

inline float Vector3f::recipLengthEst() const
{
	return XMVectorGetX(XMVector3ReciprocalLengthEst(XMLoadFloat3(this)));
}

inline float Vector3f::angleRad(const Vector3f &p) const
{
	return XMVectorGetX(XMVector3AngleBetweenVectors(XMLoadFloat3(this), XMLoadFloat3(&p)));
}

inline float Vector3f::angleRadUnit(const Vector3f &p) const
{
	return XMVectorGetX(XMVector3AngleBetweenNormals(XMLoadFloat3(this), XMLoadFloat3(&p)));
}

inline float Vector3f::angleRadUnitEst(const Vector3f &p) const
{
	return XMVectorGetX(XMVector3AngleBetweenNormalsEst(XMLoadFloat3(this), XMLoadFloat3(&p)));
}

inline string Vector3f::toString() const
{
	ostringstream returnStr;
	returnStr << "(" << x << "," << y << "," << z << ")";
	return returnStr.str();
}

// Constructors / Destructor
inline Vector3f::Vector3f() :
	XMFLOAT3(0, 0, 0)
{}

inline Vector3f::Vector3f(const Vector3f &p) :
	XMFLOAT3(p.x, p.y, p.z)
{}

inline Vector3f::Vector3f(const float _x, const float _y, const float _z) :
	XMFLOAT3(_x, _y, _z)
{}
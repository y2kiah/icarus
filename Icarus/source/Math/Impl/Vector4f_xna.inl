/* Vector4f_xna.h
Author: Jeff Kiah
Orig.Date: 5/21/12
*/
#pragma once

#include "Math/Vector4f.h"
#include <sstream>

using std::ostringstream;

// Operators

inline bool Vector4f::operator==(const Vector4f &p) const
{
	return (XMVector4Equal(XMLoadFloat4(this), XMLoadFloat4(&p)) != FALSE);
}

inline bool Vector4f::operator!=(const Vector4f &p) const
{
	return (XMVector4NotEqual(XMLoadFloat4(this), XMLoadFloat4(&p)) != FALSE);
}

inline void Vector4f::operator= (const Vector4f &p) { assign(p); }
inline void Vector4f::operator+=(const Vector4f &p) { add(p); }
inline void Vector4f::operator-=(const Vector4f &p) { subtract(p); }
inline void Vector4f::operator*=(const Vector4f &p)	{ multiply(p); }
inline void Vector4f::operator/=(const Vector4f &p)	{ divide(p); }
inline void Vector4f::operator+=(const float s)		{ add(s); }
inline void Vector4f::operator-=(const float s)		{ subtract(s); }
inline void Vector4f::operator*=(const float s)		{ scale(s); }
inline void Vector4f::operator/=(const float s)		{ divide(s); }

inline Vector4f Vector4f::operator- () const
{
	return Vector4f(-x, -y, -z, -w);
}

inline Vector4f Vector4f::operator+ (const float s) const
{
	Vector4f v(*this);
	return v.add(s);
}

inline Vector4f Vector4f::operator- (const float s) const
{
	Vector4f v(*this);
	return v.subtract(s);
}

inline Vector4f Vector4f::operator* (const float s) const
{
	Vector4f v(*this);
	return v.scale(s);
}

inline Vector4f Vector4f::operator/ (const float s) const
{
	Vector4f v(*this);
	return v.divide(s);
}

inline Vector4f Vector4f::operator+ (const Vector4f &p) const
{
	Vector4f v(*this);
	return v.add(p);
}

inline Vector4f Vector4f::operator- (const Vector4f &p) const
{
	Vector4f v(*this);
	return v.subtract(p);
}

inline Vector4f Vector4f::operator* (const Vector4f &p) const
{
	Vector4f v(*this);
	return v.multiply(p);
}

inline Vector4f Vector4f::operator/ (const Vector4f &p) const
{
	Vector4f v(*this);
	return v.divide(p);
}

// Functions

inline void Vector4f::assign(const float _x, const float _y,
							 const float _z, const float _w)
{
	x = _x; y = _y; z = _z; w = _w;
}

inline void Vector4f::assign(const Vector4f &p)
{
	x = p.x; y = p.y; z = p.z; w = p.w;
}

inline bool Vector4f::nearEqualTo(const Vector4f &p) const
{
	return (XMVector4NearEqual(XMLoadFloat4(this), XMLoadFloat4(&p), XMVectorSplatEpsilon()) != FALSE);
}

inline void Vector4f::add(const Vector4f &p1,const Vector4f &p2)
{
	XMStoreFloat4(	this,
					XMVectorAdd(XMLoadFloat4(&p1), XMLoadFloat4(&p2)));
}

inline Vector4f & Vector4f::add(const Vector4f &p)
{
	add(*this, p);
	return *this;
}

inline void Vector4f::subtract(const Vector4f &p1,const Vector4f &p2)
{
	XMStoreFloat4(	this,
					XMVectorSubtract(XMLoadFloat4(&p1), XMLoadFloat4(&p2)));
}

inline Vector4f & Vector4f::subtract(const Vector4f &p)
{
	subtract(*this, p);
	return *this;
}

inline void Vector4f::multiply(const Vector4f &p1, const Vector4f &p2)
{
	XMStoreFloat4(	this,
					XMVectorMultiply(XMLoadFloat4(&p1), XMLoadFloat4(&p2)));
}

inline Vector4f & Vector4f::multiply(const Vector4f &p)
{
	multiply(*this, p);
	return *this;
}

inline void Vector4f::divide(const Vector4f &p1, const Vector4f &p2)
{
	XMStoreFloat4(	this,
					XMVectorDivide(XMLoadFloat4(&p1), XMLoadFloat4(&p2)));
}

inline Vector4f & Vector4f::divide(const Vector4f &p) {
	divide(*this, p);
	return *this;
}

inline void Vector4f::cross(const Vector4f &p1, const Vector4f &p2, const Vector4f &p3)
{
	XMStoreFloat4(	this,
					XMVector4Cross(XMLoadFloat4(&p1), XMLoadFloat4(&p2), XMLoadFloat4(&p3)));
}

inline Vector4f & Vector4f::cross(const Vector4f &p1, const Vector4f &p2)
{
	cross(*this, p1, p2);
	return *this;
}

inline void Vector4f::lerp(const Vector4f &p1, const Vector4f &p2, const float t)
{
	XMStoreFloat4(	this,
					XMVectorLerp(XMLoadFloat4(&p1), XMLoadFloat4(&p2), t));
}

inline void Vector4f::lerpV(const Vector4f &p1, const Vector4f &p2, const Vector4f &t)
{
	XMStoreFloat4(	this,
					XMVectorLerpV(XMLoadFloat4(&p1), XMLoadFloat4(&p2), XMLoadFloat4(&t)));
}

inline Vector4f & Vector4f::lerp(const Vector4f &p, const float t)
{
	lerp(*this, p, t);
	return *this;
}

inline Vector4f & Vector4f::lerpV(const Vector4f &p, const Vector4f &t)
{
	lerpV(*this, p, t);
	return *this;
}

inline void Vector4f::add(const Vector4f &p, const float s)
{
	XMStoreFloat4(	this,
					XMVectorAdd(XMLoadFloat4(&p), XMVectorReplicate(s)));
}

inline Vector4f & Vector4f::add(const float s)
{
	add(*this, s);
	return *this;
}

inline void Vector4f::subtract(const Vector4f &p, const float s)
{
	XMStoreFloat4(	this,
					XMVectorSubtract(XMLoadFloat4(&p), XMVectorReplicate(s)));
}

inline Vector4f & Vector4f::subtract(const float s)
{
	subtract(*this, s);
	return *this;
}

inline void Vector4f::scale(const Vector4f &p, const float s)
{
	XMStoreFloat4(	this,
					XMVectorScale(XMLoadFloat4(&p), s));
}

inline Vector4f & Vector4f::scale(const float s)
{
	scale(*this, s);
	return *this;
}

inline void Vector4f::divide(const Vector4f &p, const float s)
{
	XMStoreFloat4(	this,
					XMVectorDivide(XMLoadFloat4(&p), XMVectorReplicate(s)));
}

inline Vector4f & Vector4f::divide(float s)
{
	divide(*this, s);
	return *this;
}

inline void Vector4f::unitNormalOf(const Vector4f &p1, const Vector4f &p2)
{
	cross(p1, p2);	// Calculates the normal vector with cross product
	normalize();	// Normalizes the vector
}

inline Vector4f & Vector4f::normalize()
{
	XMStoreFloat4(	this,
					XMVector4Normalize(XMLoadFloat4(this)));
	return *this;
}

inline Vector4f & Vector4f::normalizeEst()
{
	XMStoreFloat4(	this,
					XMVector4NormalizeEst(XMLoadFloat4(this)));
	return *this;
}

// Reflects an incident 4D vector across a 4D normal vector
inline void Vector4f::reflect(const Vector4f &incident, const Vector4f &normal)
{
	XMStoreFloat4(	this,
					XMVector4Reflect(XMLoadFloat4(&incident), XMLoadFloat4(&normal)));
}

inline Vector4f & Vector4f::reflect(const Vector4f &normal)
{
	reflect(*this, normal);
	return *this;
}

// Refracts an incident 4D vector across a 4D normal vector
inline void Vector4f::refract(	const Vector4f &incident, const Vector4f &normal,
								const float refractionIndex)
{
	XMStoreFloat4(	this,
					XMVector4Refract(XMLoadFloat4(&incident), XMLoadFloat4(&normal), refractionIndex));
}

inline Vector4f & Vector4f::refract(const Vector4f &normal, const float refractionIndex)
{
	refract(*this, normal, refractionIndex);
	return *this;
}

inline float Vector4f::dot(const Vector4f &p) const
{
	return XMVectorGetX(XMVector4Dot(XMLoadFloat4(this), XMLoadFloat4(&p)));
}

inline float Vector4f::distanceTo(const Vector4f &p) const
{
	XMVECTOR v(XMVectorSubtract(XMLoadFloat4(this), XMLoadFloat4(&p)));
	return XMVectorGetX(XMVector4Length(v));
}

inline float Vector4f::distanceToEst(const Vector4f &p) const
{
	XMVECTOR v(XMVectorSubtract(XMLoadFloat4(this), XMLoadFloat4(&p)));
	return XMVectorGetX(XMVector4LengthEst(v));
}

inline float Vector4f::distSquaredTo(const Vector4f &p) const
{
	XMVECTOR v(XMVectorSubtract(XMLoadFloat4(this), XMLoadFloat4(&p)));
	return XMVectorGetX(XMVector4LengthSq(v));
}

inline float Vector4f::length() const
{
	return XMVectorGetX(XMVector4Length(XMLoadFloat4(this)));
}

inline float Vector4f::lengthEst() const
{
	return XMVectorGetX(XMVector4LengthEst(XMLoadFloat4(this)));
}

inline float Vector4f::lengthSquared() const
{
	return XMVectorGetX(XMVector4LengthSq(XMLoadFloat4(this)));
}

inline float Vector4f::recipLength() const
{
	return XMVectorGetX(XMVector4ReciprocalLength(XMLoadFloat4(this)));
}

inline float Vector4f::recipLengthEst() const
{
	return XMVectorGetX(XMVector4ReciprocalLengthEst(XMLoadFloat4(this)));
}

inline float Vector4f::angleRad(const Vector4f &p) const
{
	return XMVectorGetX(XMVector4AngleBetweenVectors(XMLoadFloat4(this), XMLoadFloat4(&p)));
}

inline float Vector4f::angleRadUnit(const Vector4f &p) const
{
	return XMVectorGetX(XMVector4AngleBetweenNormals(XMLoadFloat4(this), XMLoadFloat4(&p)));
}

inline float Vector4f::angleRadUnitEst(const Vector4f &p) const
{
	return XMVectorGetX(XMVector4AngleBetweenNormalsEst(XMLoadFloat4(this), XMLoadFloat4(&p)));
}

inline string Vector4f::toString() const
{
	ostringstream returnStr;
	returnStr << "(" << x << "," << y << "," << z << "," << w << ")";
	return returnStr.str();
}

// Constructors / Destructor
inline Vector4f::Vector4f() :
	XMFLOAT4(0, 0, 0, 0)
{}

inline Vector4f::Vector4f(const Vector4f &p) :
	XMFLOAT4(p.x, p.y, p.z, p.w)
{}

inline Vector4f::Vector4f(const float _x, const float _y,
						  const float _z, const float _w) :
	XMFLOAT4(_x, _y, _z, _w)
{}
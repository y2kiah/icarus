/* Vector3f_acml.h
Author: Jeff Kiah
Orig.Date: 5/20/12
*/
#pragma once

#include "Math/Vector3f.h"
#include <sstream>
#include <cmath>
#include "Math/MathDefines.h"

using std::ostringstream;

// Operators

inline bool Vector3f::operator==(const Vector3f &p) const
{
	return (x == p.x && y == p.y && z == p.z);
}

inline bool Vector3f::operator!=(const Vector3f &p) const
{
	return (x != p.x || y != p.y || z != p.z);
}

inline void Vector3f::operator= (const Vector3f &p) { assign(p); }
inline void Vector3f::operator+=(const Vector3f &p) { add(p); }
inline void Vector3f::operator-=(const Vector3f &p) { subtract(p); }
inline void Vector3f::operator+=(const float s) { add(s); }
inline void Vector3f::operator-=(const float s) { subtract(s); }
inline void Vector3f::operator*=(const float s) { multiply(s); }
inline void Vector3f::operator/=(const float s) { divide(s); }

inline Vector3f Vector3f::operator- () const { return Vector3f(-x,-y,-z); } // unary
inline Vector3f Vector3f::operator+ (const float s) const { return Vector3f(x+s, y+s, z+s); }
inline Vector3f Vector3f::operator- (const float s) const { return Vector3f(x-s, y-s, z-s); }
inline Vector3f Vector3f::operator* (const float s) const { return Vector3f(x*s, y*s, z*s); }

inline Vector3f Vector3f::operator/ (float s) const
{
	_ASSERTE(s != 0.0f);
	s = 1.0f / s;
	return Vector3f(x*s, y*s, z*s);
}

inline Vector3f Vector3f::operator+ (const Vector3f &p) const { return Vector3f(x+p.x, y+p.y, z+p.z); }
inline Vector3f Vector3f::operator- (const Vector3f &p) const { return Vector3f(x-p.x, y-p.y, z-p.z); }
inline Vector3f Vector3f::operator* (const Vector3f &p) const { return Vector3f(y*p.z-z*p.y, z*p.x-x*p.z, x*p.y-y*p.x); }

// Functions

inline void Vector3f::assign(float _x, float _y, float _z) { x = _x; y = _y; z = _z; }
inline void Vector3f::assign(const Vector3f &p) { x = p.x; y = p.y; z = p.z; }

inline bool Vector3f::equalTo(const Vector3f &p, const float epsilon) const
{
	if (fabs(p.x - x) <= epsilon) {
		if (fabs(p.y - y) <= epsilon) {
			if (fabs(p.z - z) <= epsilon) {
				return true;
			}
		}
	}
	return false;
}

inline bool Vector3f::notEqualTo(const Vector3f &p, const float epsilon) const
{
	if ((fabs(p.x-x) > epsilon) || (fabs(p.y-y) > epsilon) || (fabs(p.z-z) > epsilon)) {
		return true;
	}
	return false;
}

inline void Vector3f::add(const Vector3f &p1,const Vector3f &p2)
{
	x = p1.x + p2.x; y = p1.y + p2.y; z = p1.z + p2.z;
}

inline Vector3f & Vector3f::add(const Vector3f &p)
{
	x += p.x; y += p.y; z += p.z;
	return *this;
}

inline void Vector3f::subtract(const Vector3f &p1,const Vector3f &p2)
{
	x = p1.x - p2.x; y = p1.y - p2.y; z = p1.z - p2.z;
}

inline Vector3f & Vector3f::subtract(const Vector3f &p)
{
	x -= p.x; y -= p.y; z -= p.z;
	return *this;
}

inline void Vector3f::cross(const Vector3f &p1, const Vector3f &p2)
{
	x = (p1.y * p2.z) - (p1.z * p2.y);
	y = (p1.z * p2.x) - (p1.x * p2.z);
	z = (p1.x * p2.y) - (p1.y * p2.x);
}

inline Vector3f & Vector3f::cross(const Vector3f &p)
{
	assign(	y * p.z - z * p.y,
			z * p.x - x * p.z,
			x * p.y - y * p.x);
	return *this;
}

inline void Vector3f::lerp(const Vector3f &p1, const Vector3f &p2, const float t)
{
	x = Math::lerp(p1.x, p2.x, t);
	y = Math::lerp(p1.y, p2.y, t);
	z = Math::lerp(p1.z, p2.z, t);
}

inline Vector3f & Vector3f::lerp(const Vector3f &p, const float t)
{
	assign(	Math::lerp(x, p.x, t),
			Math::lerp(y, p.y, t),
			Math::lerp(z, p.z, t));
	return *this;
}

inline void Vector3f::add(const Vector3f &p, const float s)
{
	x = p.x + s; y = p.y + s; z = p.z + s;
}

inline Vector3f & Vector3f::add(const float s)
{
	x += s; y += s; z += s;
	return *this;
}

inline void Vector3f::subtract(const Vector3f &p, const float s)
{
	x = p.x - s; y = p.y - s; z = p.z - s;
}

inline Vector3f & Vector3f::subtract(const float s)
{
	x -= s; y -= s; z -= s;
	return *this;
}

inline void Vector3f::multiply(const Vector3f &p, const float s)
{
	x = p.x * s; y = p.y * s; z = p.z * s;
}

inline Vector3f & Vector3f::multiply(const float s)
{
	x *= s; y *= s; z *= s;
	return *this;
}

inline void Vector3f::divide(const Vector3f &p, float s)
{
	_ASSERTE(s != 0.0f);
	s = 1.0f / s;
	x = p.x * s; y = p.y * s; z = p.z * s;
}

inline Vector3f & Vector3f::divide(float s)
{
	_ASSERTE(s != 0.0f);
	s = 1.0f / s;
	x *= s; y *= s; z *= s;
	return *this;
}

inline float Vector3f::dot(const Vector3f &p) const
{
	return x*p.x + y*p.y + z*p.z;
}

inline float Vector3f::distance(const Vector3f &p) const
{
	const float dx = p.x - x;
	const float dy = p.y - y;
	const float dz = p.z - z;
	return sqrtf(dx*dx + dy*dy + dz*dz);
}

inline float Vector3f::distSquared(const Vector3f &p) const
{
	const float dx = p.x - x;
	const float dy = p.y - y;
	const float dz = p.z - z;
	return dx*dx + dy*dy + dz*dz;
}

inline float Vector3f::magnitude() const
{
	return sqrtf(x*x + y*y + z*z);
}

inline float Vector3f::magSquared() const
{
	return x*x + y*y + z*z;
}

inline void Vector3f::unitNormalOf(const Vector3f &p1, const Vector3f &p2)
{
	// Calculates the normal vector with cross product
	cross(p1, p2);
	// Normalizes the vector
	normalize();
}

inline void Vector3f::normalize()
{
	float mag = magnitude();
	_ASSERTE(mag != 0.0f);
	float invMag = 1.0f / mag;
	x *= invMag; y *= invMag; z *= invMag;
}

inline float Vector3f::angleRad(const Vector3f &p) const
{
	return acosf(static_cast<float>(dot(p) / (magnitude() * p.magnitude())));
}

inline float Vector3f::angleDeg(const Vector3f &p) const
{
	return angleRad(p) * RADTODEGf;
}

inline float Vector3f::angleRadUnit(const Vector3f &p) const
{
	return acosf(static_cast<float>(dot(p)));
}

inline float Vector3f::angleDegUnit(const Vector3f &p) const
{
	return angleRadUnit(p) * RADTODEGf;
}

inline void Vector3f::rotate(Vector3f &p, uint32_t xa, uint32_t ya, uint32_t za)
{
/*	*this = p;
	Vector3f tp;
	
	tp.x = x * math.getCos(za) + y * math.getSin(za);
	tp.y = y * math.getCos(za) - x * math.getSin(za);
	x = tp.x;
	y = tp.y;
	
	tp.y = y * math.getCos(xa) + z * math.getSin(xa);
	tp.z = z * math.getCos(xa) - y * math.getSin(xa);
	y = tp.y;
	z = tp.z;

	tp.z = z * math.getCos(ya) + x * math.getSin(ya);
	tp.x = x * math.getCos(ya) - z * math.getSin(ya);
	x = tp.x;
	z = tp.z;
*/
}

inline void Vector3f::rotate(uint32_t xa, uint32_t ya, uint32_t za)
{
/*	Vector3f tp;
	
	tp.x = x * math.getCos(za) + y * math.getSin(za);
	tp.y = y * math.getCos(za) - x * math.getSin(za);
	x = tp.x;
	y = tp.y;
		
	tp.y = y * math.getCos(xa) + z * math.getSin(xa);
	tp.z = z * math.getCos(xa) - y * math.getSin(xa);
	y = tp.y;
	z = tp.z;

	tp.z = z * math.getCos(ya) + x * math.getSin(ya);
	tp.x = x * math.getCos(ya) - z * math.getSin(ya);
	x = tp.x;
	z = tp.z;
*/
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

inline Vector3f::Vector3f(float _x, float _y, float _z) :
	XMFLOAT3(_x, _y, _z)
{}
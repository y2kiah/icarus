/* Vector3f.h
Author: Jeff Kiah
Orig.Date: 5/20/12
*/
#pragma once

#include <string>

#if defined(WIN32)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN	// defined in project settings
#endif
#include <windows.h>
#include <xnamath.h>
#endif

using std::string;

// class Vector3f

class Vector3f
#if defined(WIN32)
	: public XMFLOAT3
#endif
{
	public:
		// Variables
#if !defined(WIN32)
		union {
			struct {
				float x, y, z;	// This union allows access to the same memory location through either
			};					// x,y,z or the array v[3]
			float v[3];			// v can be used for fast OpenGL functions
		};
#endif
		// Operators

		// Conditional operators
		inline bool operator==(const Vector3f &p) const;
		inline bool operator!=(const Vector3f &p) const;

		// Assignment operators
		// Vector operations
		inline void operator= (const Vector3f &p); // set equal to p
		inline void operator+=(const Vector3f &p); // add by corresponding components of p
		inline void operator-=(const Vector3f &p); // subtract by corresponding components of p
		inline void operator*=(const Vector3f &p); // multiply by corresponding components of p
		inline void operator/=(const Vector3f &p); // divide by corresponding components of p
		// Scalar operatations
		inline void operator+=(const float s);
		inline void operator-=(const float s);
		inline void operator*=(const float s);
		inline void operator/=(const float s);

		// Not as fast as functions below, but these allow streamed operations e.g. V = VA + VB - VC
		inline Vector3f operator- () const;
		// Vector operations
		inline Vector3f operator+ (const Vector3f &p) const;
		inline Vector3f operator- (const Vector3f &p) const;
		inline Vector3f operator* (const Vector3f &p) const;
		inline Vector3f operator/ (const Vector3f &p) const;
		// Scalar operations
		inline Vector3f operator+ (const float s) const;
		inline Vector3f operator- (const float s) const;
		inline Vector3f operator* (const float s) const;
		inline Vector3f operator/ (const float s) const;

		// Functions

		inline void assign(const float _x, const float _y, const float _z);
		inline void assign(const Vector3f &p);

		// Check equality within a threshold, slower but good for comparisons after floating point operations
		inline bool nearEqualTo(const Vector3f &p) const;

		// Vector operations

		// Faster than the overloaded operators above
		inline void add(const Vector3f &p, const float s);
		inline void subtract(const Vector3f &p, const float s);
		inline void scale(const Vector3f &p, const float s);
		inline void divide(const Vector3f &p, const float s);
		inline void add(const Vector3f &p1, const Vector3f &p2);
		inline void subtract(const Vector3f &p1, const Vector3f &p2);
		inline void multiply(const Vector3f &p1, const Vector3f &p2);
		inline void divide(const Vector3f &p1, const Vector3f &p2);
		inline void cross(const Vector3f &p1, const Vector3f &p2);
		inline void lerp(const Vector3f &p1, const Vector3f &p2, const float t);
		inline void lerpV(const Vector3f &p1, const Vector3f &p2, const Vector3f &t);
		inline void unitNormalOf(const Vector3f &p1, const Vector3f &p2); // set to the unit normal of two vectors
		inline void reflect(const Vector3f &incident, const Vector3f &normal);	// Reflects an incident 3D vector across a 3D normal vector
		inline void refract(const Vector3f &incident, const Vector3f &normal,	// Refracts an incident 3D vector across a 3D normal vector
							const float refractionIndex);

		// Fast operators for chaining commands
		inline Vector3f & add(const float s);
		inline Vector3f & subtract(const float s);
		inline Vector3f & scale(const float s);
		inline Vector3f & divide(const float s);
		inline Vector3f & add(const Vector3f &p);
		inline Vector3f & subtract(const Vector3f &p);
		inline Vector3f & multiply(const Vector3f &p);
		inline Vector3f & divide(const Vector3f &p);
		inline Vector3f & cross(const Vector3f &p);
		inline Vector3f & lerp(const Vector3f &p, const float t);
		inline Vector3f & lerpV(const Vector3f &p, const Vector3f &t);
		inline Vector3f & normalize(); // normalize this vector to unit length
		inline Vector3f & normalizeEst(); // faster version of the above function, trades accuracy for speed
		inline Vector3f & reflect(const Vector3f &normal);
		inline Vector3f & refract(const Vector3f &normal, const float refractionIndex);

		// Scalar operations

		// Dot product
		inline float dot(const Vector3f &p) const;
		// Distance
		inline float distanceTo(const Vector3f &p) const; // distance to another point
		inline float distanceToEst(const Vector3f &p) const; // faster version of the above function, trades accuracy for speed
		inline float distSquaredTo(const Vector3f &p) const;
		inline float linePointDistance(const Vector3f &lp1, const Vector3f &lp2); // Computes the minimum distance to a line
		// Finds the length (magnitude) of the vector. Distance of a point from the origin
		inline float length() const;
		inline float lengthEst() const; // faster version of the above function, trades accuracy for speed
		inline float lengthSquared() const;
		// Reciprocal of the length
		inline float recipLength() const;
		inline float recipLengthEst() const; // faster version of the above function, trades accuracy for speed
		// Get angle to another vector
		inline float angleRad(const Vector3f &p) const;
		inline float angleRadUnit(const Vector3f &p) const; // assumes both vectors are already unit (length = 1.0), does not normalize the vectors
		inline float angleRadUnitEst(const Vector3f &p) const; // faster version of the above function, trades accuracy for speed

		// Debugging
		inline string toString() const;

		// Constructors
		inline Vector3f();
		inline Vector3f(const Vector3f &p);
		inline explicit Vector3f(const float _x, const float _y, const float _z);
};

#if defined(WIN32)
#include "Impl/Vector3f_xna.inl"
#else
#include "Impl/Vector3f_acml.inl"
#endif
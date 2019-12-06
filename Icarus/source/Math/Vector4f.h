/* Vector4f.h
Author: Jeff Kiah
Orig.Date: 5/21/12
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

// class Vector4f

class Vector4f
#if defined(WIN32)
	: public XMFLOAT4
#endif
{
	public:
		// Variables
#if !defined(WIN32)
		union {
			struct {
				float x, y, z, w;	// This union allows access to the same memory location through either
			};						// x,y,z,w or the array v[4]
			float v[4];				// v can be used for fast OpenGL functions
		};
#endif
		// Operators

		// Comparison operators
		inline bool operator==(const Vector4f &p) const;
		inline bool operator!=(const Vector4f &p) const;

		// Assignment operators
		// Vector operations
		inline void operator= (const Vector4f &p); // set equal to p
		inline void operator+=(const Vector4f &p); // add by corresponding components of p
		inline void operator-=(const Vector4f &p); // subtract by corresponding components of p
		inline void operator*=(const Vector4f &p); // multiply by corresponding components of p
		inline void operator/=(const Vector4f &p); // divide by corresponding components of p
		// Scalar operatations
		inline void operator+=(const float s);
		inline void operator-=(const float s);
		inline void operator*=(const float s);
		inline void operator/=(const float s);

		// Not as fast as functions below, but these allow streamed operations e.g. V = VA + VB - VC
		inline Vector4f operator- () const;
		// Vector operations
		inline Vector4f operator+ (const Vector4f &p) const;
		inline Vector4f operator- (const Vector4f &p) const;
		inline Vector4f operator* (const Vector4f &p) const;
		inline Vector4f operator/ (const Vector4f &p) const;
		// Scalar operations
		inline Vector4f operator+ (const float s) const;
		inline Vector4f operator- (const float s) const;
		inline Vector4f operator* (const float s) const;
		inline Vector4f operator/ (const float s) const;

		// Functions

		// Comparison Functions
		// Check equality within a threshold, slower but good for comparisons after floating point operations
		inline bool nearEqualTo(const Vector4f &p) const;

		// Assignment Functions
		inline void assign(	const float _x, const float _y,
							const float _z, const float _w);
		inline void assign(const Vector4f &p);

		// Vector operations

		// Faster than the overloaded operators above
		inline void add(const Vector4f &p, const float s);
		inline void subtract(const Vector4f &p, const float s);
		inline void scale(const Vector4f &p, const float s);
		inline void divide(const Vector4f &p, const float s);
		inline void add(const Vector4f &p1, const Vector4f &p2);
		inline void subtract(const Vector4f &p1, const Vector4f &p2);
		inline void multiply(const Vector4f &p1, const Vector4f &p2);
		inline void divide(const Vector4f &p1, const Vector4f &p2);
		inline void cross(const Vector4f &p1, const Vector4f &p2, const Vector4f &p3);
		inline void lerp(const Vector4f &p1, const Vector4f &p2, const float t);
		inline void lerpV(const Vector4f &p1, const Vector4f &p2, const Vector4f &t);
		inline void unitNormalOf(const Vector4f &p1, const Vector4f &p2); // set to the unit normal of two vectors
		inline void reflect(const Vector4f &incident, const Vector4f &normal);	// Reflects an incident 4D vector across a 4D normal vector
		inline void refract(const Vector4f &incident, const Vector4f &normal,	// Refracts an incident 4D vector across a 4D normal vector
							const float refractionIndex);

		// Fast operators for chaining commands
		inline Vector4f & add(const float s);
		inline Vector4f & subtract(const float s);
		inline Vector4f & scale(const float s);
		inline Vector4f & divide(const float s);
		inline Vector4f & add(const Vector4f &p);
		inline Vector4f & subtract(const Vector4f &p);
		inline Vector4f & multiply(const Vector4f &p);
		inline Vector4f & divide(const Vector4f &p);
		inline Vector4f & cross(const Vector4f &p1, const Vector4f &p2);
		inline Vector4f & lerp(const Vector4f &p, const float t);
		inline Vector4f & lerpV(const Vector4f &p, const Vector4f &t);
		inline Vector4f & normalize(); // normalize this vector to unit length
		inline Vector4f & normalizeEst(); // faster version of the above function, trades accuracy for speed
		inline Vector4f & reflect(const Vector4f &normal);
		inline Vector4f & refract(const Vector4f &normal, const float refractionIndex);

		// Scalar operations

		// Dot product
		inline float dot(const Vector4f &p) const;
		// Distance
		inline float distanceTo(const Vector4f &p) const; // distance to another point
		inline float distanceToEst(const Vector4f &p) const; // faster version of the above function, trades accuracy for speed
		inline float distSquaredTo(const Vector4f &p) const;
		// Finds the length (magnitude) of the vector. Distance of a point from the origin
		inline float length() const;
		inline float lengthEst() const; // faster version of the above function, trades accuracy for speed
		inline float lengthSquared() const;
		// Reciprocal of the length
		inline float recipLength() const;
		inline float recipLengthEst() const; // faster version of the above function, trades accuracy for speed
		// Get angle to another vector
		inline float angleRad(const Vector4f &p) const;
		inline float angleRadUnit(const Vector4f &p) const; // assumes both vectors are already unit (length = 1.0), does not normalize the vectors
		inline float angleRadUnitEst(const Vector4f &p) const; // faster version of the above function, trades accuracy for speed

		// Debugging
		inline string toString() const;

		// Constructors
		inline Vector4f();
		inline Vector4f(const Vector4f &p);
		inline explicit Vector4f(const float _x, const float _y,
								 const float _z, const float _w);
};

#if defined(WIN32)
#include "Impl/Vector4f_xna.inl"
#else
#include "Impl/Vector3f_acml.inl"
#endif
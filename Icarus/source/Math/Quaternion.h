/* Quaternion.h
Author:	Jeff Kiah
Orig.Date: 5/21/12
*/
#pragma once

#include "Vector4f.h"

class Vector3f;
class Matrix4x4f;

// class Quaternion

class Quaternion : public Vector4f {
	public:
		// Operators - Overrides Vector4f operators
		inline Quaternion operator* (const Quaternion &q) const;
		inline Quaternion operator* (const float s) const;

		// Functions

		// Assignment Functions
		inline void setIdentity();	// set quaternion to identity
		inline void setConjugate();	// calculate conjugate of quaternion
		inline void	setInverse();	// calculate inverse of quaternion
		
		// Faster than the overloaded operators above - Overrides Vector4f functions
		inline void scale(const Quaternion &q, const float s);
		inline void multiply(const Quaternion &q0, const Quaternion &q1); // contatenate Quaternions used for rotation

		// Fast operators for chaining commands - Overrides Vector4f functions
		inline Quaternion & scale(const float s);
		inline Quaternion & multiply(const Quaternion &q); // contatenate Quaternions used for rotation

		// Interpolation
		inline Quaternion & nlerp(const Quaternion &q0, const Quaternion &q1, const float t);
		inline Quaternion & slerp(const Quaternion &q0, const Quaternion &q1, const float t);
		inline Quaternion & squad(const Quaternion &q0, const Quaternion &q1,
								  const Quaternion &q2, const Quaternion &q3, const float t);
		// Rotation
		inline Quaternion & setRotationAxis(const Vector3f &axis, const float angle);
		inline Quaternion & setRotationNormal(const Vector3f &normAxis, const float angle);
		inline Quaternion & setRotationFromMatrix(const Matrix4x4f &m);
		inline Quaternion & setRotationRollPitchYaw(float p, float y, float r);
		inline Quaternion & setRotationRollPitchYawV(const Vector3f &v);

		// Conversion
		inline void toAxisAngle(Vector3f &axis, float &angle) const; // convert quaternion to axis-angle

		// Constructors
		inline Quaternion();
		inline Quaternion(const Quaternion &q); // copy constructor
		inline explicit Quaternion(	const float _x, const float _y,
									const float _z, const float _w); // construct quaternion from real component w and imaginary x,y,z
		inline explicit Quaternion(const Vector3f &axis, const float angle); // construct quaternion from axis-angle
		inline explicit Quaternion(const Matrix4x4f &m); // construct quaternion from rotation matrix
};

#if defined(WIN32)
#include "Impl/Quaternion_xna.inl"
#else
#include "Impl/Quaternion_acml.inl"
#endif
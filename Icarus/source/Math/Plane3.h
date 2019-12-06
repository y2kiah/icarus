/* Plane3.h
Author: Jeff Kiah
Orig.Date: 5/21/12
*/
#pragma once;

#include "Math/Vector3f.h"
#include "Math/Vector4f.h"

class Matrix4x4f;

// class Plane

class Plane3 {
	public:
		// Variables
		union {
			struct {		// Ax + By + Cz + D = 0
				Vector3f n;	// plane normal
				float    d;	// D = distance from origin
			};
			struct {
				Vector4f c; // plane components, x=A, y=B, z=C, w=D
			};
		};

		// Operators
		
		// Conditional operators
		inline bool operator==(const Plane3 &p) const;
		inline bool operator!=(const Plane3 &p) const;

		// Functions
		inline float	findDist(const Vector3f &p) const { return n.dot(p) - d; }
		
		inline void		set(const Vector3f &p1, const Vector3f &p2, const Vector3f &p3);
		inline void		set(const Vector3f &p, const Vector3f &unitNormal);
		inline void		transform(const Plane3 &p, const Matrix4x4f &m); // transform plane by given Matrix

		inline Plane3 &	transform(const Matrix4x4f &m);

		// Constructors
		inline explicit Plane3();
		inline explicit Plane3(const Vector3f &p1, const Vector3f &p2, const Vector3f &p3);
		inline explicit Plane3(const Vector3f &p, const Vector3f &unitNormal);
		inline explicit Plane3(const Vector3f &unitNormal, float _d);
		inline explicit Plane3(const Vector4f &_c);
		inline explicit Plane3(const Plane3 &p);
};

#if defined(WIN32)
#include "Impl/Plane3_xna.inl"
#endif
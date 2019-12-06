/* IMath_xna.inl
Author: Jeff Kiah
Orig.Date: 5/22/12
*/

#include "Math/IMath.h"

#if defined(WIN32)

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN	// defined in project settings
#endif
#include <windows.h>
#include <crtdbg.h>
#include <xnamath.h>

#elif
#define _ASSERTE(expr) ((void)0)
#endif

namespace IMath {

// Fast alternative functions

inline float cos(float a)
{
	return XMScalarCos(a);
}

inline float cosEst(float a)
{
	return XMScalarCosEst(a);
}

inline float acos(float a)
{
	return XMScalarACos(a);
}

inline float acosEst(float a)
{
	return XMScalarACosEst(a);
}

inline float sin(float a)
{
	return XMScalarSin(a);
}

inline float sinEst(float a)
{
	return XMScalarSinEst(a);
}

inline float asin(float a)
{
	return XMScalarASin(a);
}

inline float asinEst(float a)
{
	return XMScalarASinEst(a);
}

inline void sinCos(float *pSin, float *pCos, float a)
{
	return XMScalarSinCos(pSin, pCos, a);
}

inline void sinCosEst(float *pSin, float *pCos, float a)
{
	return XMScalarSinCosEst(pSin, pCos, a);
}

inline int32_t fastSqrti(int32_t x)
{
	if (x < 1) return 0;
	// Load the binary constant 01 00 00 ... 00, where the number of zero bits to the
	// right of the single one bit is even, and the one bit is as far left as is consistant with that condition
	int32_t squaredBit = (int32_t)((((uint32_t)~0L) >> 1) & ~(((uint32_t)~0L) >> 2));
	// Form bits of the answer
	int32_t root = 0;
	while (squaredBit > 0) {
		if (x >= (squaredBit | root)) {
			x -= (squaredBit | root);
			root >>= 1; root |= squaredBit;
		} else {
			root >>= 1;
		}
		squaredBit >>= 2; 
	}
	return root;
}

inline uint32_t fastRoundUpToPowerOfTwo(uint32_t x)
{
	uint32_t xTry = 1;
	while (xTry < x) {
		xTry <<= 1;
	}
	return xTry;
}

// Comparison functions

inline bool nearEqual(float s1, float s2)
{
	return (XMScalarNearEqual(s1, s2, EPSf) != FALSE);
}

template <typename T>
inline bool isPow2(T a)
{
	return a && !(a & (a - 1));
}

template <>
inline bool isPow2(float f)
{
	uint32_t& i = (uint32_t&)f;
	uint32_t s = i >> 31;
	uint32_t e = (i >> 23) & 0xff;
	uint32_t m = i & 0x7fffff;
	return !s && !m && e >= 127;
}

// Misc functions
template <class T>
inline T clamp(T a, T b, T x)
{
	return min(a, max(b, x));
}

inline float bias(float a, float b)
{
	return powf(a, logf(b) * INV_LN_HALFf);
}

inline float gamma(float a, float g)
{
	return powf(a, 1.0f / g);
}

inline float expose(float l, float k)
{
	return (1.0f - expf(-l * k));
}

// Interpolation functions

inline float lerp(float a, float b, float t)
{
	return a + t * (b - a);
}

inline float sCurve(float t) // Cubic S-curve = 3t^2 - 2t^3 : 2nd derivative is discontinuous at t=0 and t=1 causing visual artifacts at boundaries
{
	return t * t * (3.0f - 2.0f * t);
}

inline float qCurve(float t) // Quintic curve
{
	return t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f);
}

inline float cosCurve(float t) // Cosine curve
{
	return (1.0f - IMath::cos(t * PIf)) * 0.5f;
}

inline float cosCurveEst(float t) // faster, less accurate Cosine curve
{
	return (1.0f - IMath::cosEst(t * PIf)) * 0.5f;
}
		
inline float step(float a, float x)
{
	return static_cast<float>(x >= a);
}

inline float boxStep(float a, float b, float x)
{
	_ASSERTE(b!=a);
	return clamp(0.0f, 1.0f, (x-a)/(b-a));
}

inline float pulse(float a, float b, float x)
{
	return static_cast<float>((x >= a) - (x >= b));
}

} // namespace IMath
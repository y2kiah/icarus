/* MathDefines.h
Author: Jeff Kiah
Orig.Date: 5/21/12
*/
#pragma once

#include <cstdint>
#include <limits>

// Defines

#define PIf				3.14159265358979323846f		// PI
#define TWO_PIf			6.28318530717958647692f		// PI * 2
#define	HALF_PIf		1.57079632679489661923f		// PI / 2
#define DEGTORADf		0.01745329252f				// PI / 180
#define RADTODEGf		57.29577951308f				// 180 / PI
#define HUGE_VALf		(float)(HUGE_VAL)
#define LN_HALFf		-0.69314718055994530941f	// ln(.5)
#define INV_LN_HALFf	-1.44269504088896340736f	// 1 / ln(.5)
#define LOG_HALFf		-0.30102999566398119521f	// log(.5)
#define INV_LOG_HALFf	-3.32192809488736234787f	// 1 / log(.5)
#define INFf			std::numeric_limits<float>::infinity()	// representation of infinity
#define EPSf			std::numeric_limits<float>::epsilon()	// representation smallest value

#define PI				3.14159265358979323846
#define TWO_PI			6.28318530717958647692
#define	HALF_PI			1.57079632679489661923
#define DEGTORAD		0.01745329252
#define RADTODEG		57.29577951308
#define INF				std::numeric_limits<double>::infinity()
#define EPS				std::numeric_limits<double>::epsilon()

// Functions

namespace IMath {

// Fast alternative functions
inline float cos(float a);
inline float cosEst(float a);
inline float acos(float a);
inline float acosEst(float a);
inline float sin(float a);
inline float sinEst(float a);
inline float asin(float a);
inline float asinEst(float a);
inline void	 sinCos(float *pSin, float *pCos, float a);
inline void  sinCosEst(float *pSin, float *pCos, float a);

inline int32_t fastSqrti(int32_t x);
inline uint32_t fastRoundUpToPowerOfTwo(uint32_t x);

// Comparison functions
inline bool nearEqual(float s1, float s2);

template <class T> inline bool isPow2(T a);

// Misc Functions
template <class T> inline T clamp(T a, T b, T x);

inline float bias(float a, float b);
inline float gamma(float a, float g);
inline float expose(float l, float k);

// Interpolation Functions
inline float lerp(float a, float b, float t);
inline float sCurve(float t);	// Cubic S-curve = 3t^2 - 2t^3 : 2nd derivative is discontinuous at t=0 and t=1 causing visual artifacts at boundaries
inline float qCurve(float t);	// Quintic curve
inline float cosCurve(float t);	// Cosine curve
		
inline float step(float a, float x);
inline float boxStep(float a, float b, float x);
inline float pulse(float a, float b, float x);

} // namespace IMath

#if defined(WIN32)
#include "Impl/IMath_xna.inl"
#else
// provide another implementation (like acml)
#endif
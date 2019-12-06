/* Debug.h
Author: Jeff Kiah
Orig.Date: 5/22/12
*/
#pragma once

#if defined(_DEBUG) && defined(DEBUG_CONSOLE)
	#include <cstdio> // for printf
	#include <tchar.h>
	#define debugPrintf(...)	printf(__VA_ARGS__)
	#define debugWPrintf(...)	wprintf(__VA_ARGS__)
	#define debugTPrintf(s,...)	_tprintf(TEXT(s),__VA_ARGS__)
	#define ifDebug(...)		__VA_ARGS__
#else
	#define debugPrintf(...)
	#define debugWPrintf(...)
	#define debugTPrintf(...)
	#define ifDebug(...)
#endif

// Direct3D debug object naming
#if defined(_DEBUG)
	#define D3D_DEBUG_NAME(pObj, x)		pObj->SetPrivateData(WKPDID_D3DDebugObjectName, sizeof(x)-1, x)

#else
	#define D3D_DEBUG_NAME(...)
#endif
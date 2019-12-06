/* Matrix4x4f.cpp
Author: Jeff Kiah
Orig.Date: 06/23/12
*/
#include "Math/Matrix4x4f.h"

float pIdent[16] = {1.0f, 0.0f, 0.0f, 0.0f,
					0.0f, 1.0f, 0.0f, 0.0f,
					0.0f, 0.0f, 1.0f, 0.0f,
					0.0f, 0.0f, 0.0f, 1.0f};

Matrix4x4f Matrix4x4f::identity(pIdent);
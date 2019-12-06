/* Vertex_Defs.h
Author: Jeff Kiah
Orig.Date: 06/03/2012
*/
#pragma once

#include "Math/Vector3f.h"

#pragma pack(1)

struct Vertex_P
{
	Vector3f	pos;	// position
};

struct Vertex_PN
{
	Vector3f	pos;	// position
	Vector3f	norm;	// normal
};

struct Vertex_PN_UV
{
	Vector3f	pos;	// position
	Vector3f	norm;	// normal
	float		u, v;	// texture uv
};

struct Vertex_PNT_UV
{
	Vector3f	pos;	// position
	Vector3f	norm;	// normal
	Vector3f	tan;	// tangent
	float		u, v;	// texture uv
};

struct Vertex_PNTB_UV
{
	Vector3f	pos;	// position
	Vector3f	norm;	// normal
	Vector3f	tan;	// tangent
	Vector3f	bitan;	// bitangent
	float		u, v;	// texture uv
};

#pragma pack()
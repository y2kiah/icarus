/* RenderEntry.h
Author: Jeff Kiah
Orig.Date: 6/25/12
*/
#pragma once

#include <memory>
#include "Math/Matrix4x4f.h"
#include "Effect.h"

using std::shared_ptr;

class Mesh;
class Material;

typedef shared_ptr<Mesh> MeshPtr;
typedef shared_ptr<Material> MaterialPtr;

struct RenderEntry {
	Matrix4x4f	toWorld;
	MeshPtr		mesh;
	EffectPtr	effect;
	MaterialPtr	material;
};
#ifndef __STATE_H__
#define __STATE_H__

#include "world.h"

#include "lang.h"

#include "ext/raylib.h"
#include "ext/raymath.h"
#include "ext/frustum.h"

#define VISUALS_ARRSZ 16

#define VISUALS_FLAG_WIREMODE 0x1u

typedef struct visual_data {
	Shader shaders[VISUALS_ARRSZ];
	u32 shaders_size;

	MaterialMap matmaps[VISUALS_ARRSZ][MATERIAL_MAP_BRDF + 1];
	u32 matmaps_size;

	Material materials[VISUALS_ARRSZ];
	u32 materials_size;

	Texture textures[VISUALS_ARRSZ];
	u32 textures_size;

	u32 flags;
} VisualData;

typedef struct state {
	Camera3D camera;
	Frustum camera_frustum;
	Vector3 last_loader_position;

	World world;
} GameState;

#endif // __STATE_H__
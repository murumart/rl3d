#include "lang.h"

#define STB_DS_IMPLEMENTATION
#include "ext/stb_ds.h"

#define FRUSTUM_IMPLEMENTATION
#include "ext/frustum.h"

#include "ext/raylib.h"
#include "ext/raymath.h"
#include "ext/rlgl.h"

#include "chunks.h"
#include "world.h"

#include <stdio.h>

void asserts();

i32 main(i32 argc, char **argv)
{
	InitWindow(1280, 720, "3D");
	SetWindowState(FLAG_WINDOW_RESIZABLE);

	asserts();

	Camera3D camera = { 0 };
	camera.position = (Vector3){ 10.0f, 10.0f, 10.0f };
	camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
	camera.fovy = 60.0f;
	camera.projection = CAMERA_PERSPECTIVE;

	SetTargetFPS(0);

	Shader shaders[1];
	shaders[0] = LoadShader("assets/shaders/blocks.vs", "assets/shaders/blocks.fs");

	MaterialMap matmaps[1];

	Material materials[1];
	materials[0] = LoadMaterialDefault();
	materials[0].shader = shaders[0];
	materials[0].maps = matmaps;

	Texture textures[1];
	textures[0] = LoadTexture("assets/textures/blocks/atlas.png");
	matmaps[0] = (MaterialMap){ .texture = textures[0] };

	World world;
	world_init(&world);

	bool wiremode = false;

	while (!WindowShouldClose()) {
		float delta = GetFrameTime();

		if (IsKeyPressed(KEY_C)) {
			if (IsCursorHidden()) EnableCursor();
			else DisableCursor();
		} else if (IsKeyPressed(KEY_X)) {
			if (wiremode) {
				wiremode = false;
			} else {
				wiremode = true;
			}
		}

		for (u32 i = 0; i < hmlen(world.chunkmap); i++) {
			process_chunk(&world.chunkmap[i].value, delta);
		}

		UpdateCamera(&camera, CAMERA_FREE);

		BeginDrawing();
		ClearBackground(RAYWHITE);

		BeginMode3D(camera);

		Frustum frustum;
		ExtractFrustum(&frustum);

		if (wiremode) rlEnableWireMode();

		const float frustrad = CHUNK_WIDTH;
		for (u32 i = 0; i < hmlen(world.chunkmap); i++) {
			ChunkmapKV kv = world.chunkmap[i];
			BlockPosition chunkpos = blockpos_from_chunkpos(kv.value.position);
			Vector3 chunkcentre = (Vector3){ chunkpos.x + (float)CHUNK_WIDTH / 2,
							 chunkpos.y + (float)CHUNK_HEIGHT / 2,
							 chunkpos.z + (float)CHUNK_WIDTH / 2 };

			if (SphereInFrustumV(&frustum, chunkcentre, frustrad)) {
				draw_chunk(&kv.value, materials[0]);
			}
		}

		rlDisableWireMode();

		// axis mundi
		DrawCube(Vector3Zero(), 0.2f, 9000.0f, 0.2f, MAGENTA);
		DrawLine3D((Vector3){ 0, -9999, 0 }, (Vector3){ 0, 9999, 0 }, MAGENTA);
		DrawGrid(10, 1.0f);

		EndMode3D();

		char debugstrings[2][40] = { 0 };
		snprintf(&debugstrings[0][0], 40, "cam pos: (%.2f %.2f %.2f)", camera.position.x, camera.position.y,
			 camera.position.z);
		snprintf(&debugstrings[1][0], 40, "\nfps: %d", GetFPS());
		DrawText(debugstrings[0], 0, 0, 24, BLACK);
		DrawText(debugstrings[1], 0, 0, 24, BLACK);

		EndDrawing();
	}

	CloseWindow();
	return 0;
}

void asserts()
{
	BlockPosition a = { 1, 2, 3 };
	BlockPosition b = chunk_world_bpos_to_local((BlockPosition){ 1, 2, 3 });
	assert(a.x == b.x && a.y == b.y && a.z == b.z);
	b = chunk_world_bpos_to_local((BlockPosition){ 1 + 16, 2 + 16, 3 + 16 });
	assert(a.x == b.x && a.y == b.y && a.z == b.z);
	b = chunk_world_bpos_to_local((BlockPosition){ 1 - 48, 2 - 16, 3 - 32 });
	assert(a.x == b.x && a.y == b.y && a.z == b.z);

	ChunkPosition c = { 1, 1, 1 };
	ChunkPosition d = chunkpos_from_blockpos((BlockPosition){ 17, 18, 19 });
	assert(c.x == d.x && c.y == d.y && c.z == d.z);
	c = (ChunkPosition){ -1, -1, 0 };
	d = chunkpos_from_blockpos((BlockPosition){ -1, -15, 15 });
	assert(c.x == d.x && c.y == d.y && c.z == d.z);

	BLOCKS_ZYX_LOOP(CHUNK_WIDTH, CHUNK_HEIGHT, CHUNK_WIDTH)
	{
		a = (BlockPosition){ x, y, z };
		assert(block_index(x, y, z) == block_indexp(a));
	}
}

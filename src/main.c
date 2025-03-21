#include "lang.h"

#include "state.h"

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
void init_state(GameState *state);
void init_visual(VisualData *visdat);

i32 main(i32 argc, char **argv)
{
	InitWindow(1280, 720, "3D");
	SetWindowState(FLAG_WINDOW_RESIZABLE);

	asserts();

	GameState state = { 0 };
	init_state(&state);
	VisualData visdat = { 0 };
	init_visual(&visdat);

	SetTargetFPS(0);

	/* MAIN LOOP */

	while (!WindowShouldClose()) {
		/* PROCESSING */

		float delta = GetFrameTime();

		if (IsKeyPressed(KEY_C)) {
			if (IsCursorHidden()) EnableCursor();
			else DisableCursor();
		} else if (IsKeyPressed(KEY_X)) {
			if (visdat.flags & VISUALS_FLAG_WIREMODE) {
				visdat.flags &= ~VISUALS_FLAG_WIREMODE;
			} else {
				visdat.flags |= VISUALS_FLAG_WIREMODE;
			}
		}

		BeginMode3D(state.camera);
		Frustum frustum;
		ExtractFrustum(&frustum);
		EndMode3D();
		state.camera_frustum = frustum;

		state.last_loader_position = *state.world.loader_centre;
		UpdateCamera(&state.camera, CAMERA_FREE);

		world_process_loading(&state);

		chunks_meshing_process(&state, delta);
		FIU(hmlen(state.world.chunkmap)) {
			chunk_process(&state.world.chunkmap[i].value, delta);
		}

		/* DRAWING */

		BeginDrawing();
		ClearBackground(RAYWHITE);

		BeginMode3D(state.camera);

		if (visdat.flags & VISUALS_FLAG_WIREMODE) rlEnableWireMode();

		FIU(hmlen(state.world.chunkmap)) {
			ChunkmapKV kv = state.world.chunkmap[i];

			if (is_chunk_in_frustum(&kv.value, &frustum)) {
				chunk_draw(&kv.value, visdat.materials[0]);
			}
		}

		rlDisableWireMode();

		// axis mundi
		DrawCube(Vector3Zero(), 0.2f, 9000.0f, 0.2f, MAGENTA);
		DrawLine3D(
			(Vector3){ 0, -9999, 0 },
			(Vector3){ 0, 9999, 0 },
			MAGENTA
		);
		DrawGrid(10, 1.0f);

		EndMode3D();

		char debugstrings[2][40] = { 0 };
		snprintf(
			&debugstrings[0][0],
			40,
			"cam pos: (%.2f %.2f %.2f)",
			state.camera.position.x,
			state.camera.position.y,
			state.camera.position.z
		);
		snprintf(&debugstrings[1][0], 40, "\nfps: %d", GetFPS());
		DrawText(debugstrings[0], 0, 0, 24, BLACK);
		DrawText(debugstrings[1], 0, 0, 24, BLACK);

		EndDrawing();
	}

	CloseWindow();
	return 0;
}

void init_state(GameState *state)
{
	state->camera = (Camera3D){ 0 };
	state->camera.position = (Vector3){ 10.0f, 10.0f, 10.0f };
	state->camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
	state->camera.fovy = 60.0f;
	state->camera.projection = CAMERA_PERSPECTIVE;

	world_init(&state->world, state);
}

void init_visual(VisualData *visdat)
{
	visdat->shaders[visdat->shaders_size++] = LoadShader(
		"assets/shaders/blocks.vs", "assets/shaders/blocks.fs"
	);
	visdat->textures[visdat->textures_size++]
		= LoadTexture("assets/textures/blocks/atlas.png");

	visdat->materials[0] = LoadMaterialDefault();
	visdat->materials[0].shader = visdat->shaders[0];
	visdat->materials_size++;

	visdat->matmaps[visdat->matmaps_size++][MATERIAL_MAP_ALBEDO]
		= (MaterialMap){ .texture = visdat->textures[0] };
	visdat->materials[0].maps = visdat->matmaps[0];
}

void asserts()
{
	BlockPosition a = { 1, 2, 3 };
	BlockPosition b = chunk_world_bpos_to_local((BlockPosition){ 1, 2, 3 });
	assert(a.x == b.x && a.y == b.y && a.z == b.z);
	b = chunk_world_bpos_to_local((BlockPosition
	){ 1 + CHUNK_WIDTH, 2 + CHUNK_HEIGHT, 3 + CHUNK_WIDTH });
	assert(a.x == b.x && a.y == b.y && a.z == b.z);
	b = chunk_world_bpos_to_local((BlockPosition
	){ 1 - CHUNK_WIDTH * 3, 2 - CHUNK_HEIGHT * 2, 3 - CHUNK_WIDTH });
	assert(a.x == b.x && a.y == b.y && a.z == b.z);

	/* ChunkPosition c = { 1, 1, 1 };
	ChunkPosition d = chunkpos_from_blockpos((BlockPosition){ 17, 18, 19 });
	assert(c.x == d.x && c.y == d.y && c.z == d.z);
	c = (ChunkPosition){ -1, -1, 0 };
	d = chunkpos_from_blockpos((BlockPosition){ -1, -15, 15 });
	assert(c.x == d.x && c.y == d.y && c.z == d.z); */

	BLOCKS_ZYX_LOOP(CHUNK_WIDTH, CHUNK_HEIGHT, CHUNK_WIDTH)
	{
		a = (BlockPosition){ x, y, z };
		assert(block_index(x, y, z) == block_indexp(a));
	}
}

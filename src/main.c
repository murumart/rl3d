#include "lang.h"
#define STB_DS_IMPLEMENTATION
#include "ext/stb_ds.h"

#include "ext/raylib.h"
#include "ext/raymath.h"

#include "chunks.h"
#include "world.h"

#include <stdio.h>

int main(void)
{
	InitWindow(1280, 720, "3D");
	SetWindowState(FLAG_WINDOW_RESIZABLE);

	Camera3D camera = { 0 };
	camera.position = (Vector3){ 10.0f, 10.0f, 10.0f };
	camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
	camera.fovy = 60.0f;
	camera.projection = CAMERA_PERSPECTIVE;

	SetTargetFPS(120);
	DisableCursor();

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
	init_world(&world);

	while (!WindowShouldClose()) {
		float delta = GetFrameTime();

		if (IsKeyPressed(KEY_C)) {
			if (IsCursorHidden()) EnableCursor();
			else DisableCursor();
		}

		for (u32 i = 0; i < hmlen(world.chunkmap); i++) {
			process_chunk(&world.chunkmap[i].value, delta);
		}

		UpdateCamera(&camera, CAMERA_FREE);

		BeginDrawing();
		ClearBackground(RAYWHITE);

		BeginMode3D(camera);

		for (u32 i = 0; i < hmlen(world.chunkmap); i++) {
			ChunkmapKV kv = world.chunkmap[i];
			draw_chunk(&kv.value, materials[0]);
			//printf("drew chunk %u\n", i);
		}

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

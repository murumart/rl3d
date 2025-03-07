#include "chunks.h"

#include "ext/raylib.h"
#include "ext/raymath.h"
#include "ext/stb_ds.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

u32 block_index(u32 x, u32 y, u32 z)
{
	return x + y * CHUNK_WIDTH + z * CHUNK_HEIGHT * CHUNK_WIDTH;
}

Vector3 chunk_world_position(Vector3 chunkpos)
{
	return Vector3Multiply(chunkpos, CHUNK_DIM);
}

static inline void determine_block(Chunk *chunk, Vector3 wpos, u32 x, u32 y, u32 z)
{
	i32 density = ((i32)y + wpos.y * wpos.y * wpos.y);
	density = density < 0 ? 0 : density;

	u32 i = block_index(x, y, z);
	if (GetRandomValue(0, density) == 0) {
		chunk->block_data[i] = 1;
	}
}

void init_chunk(Chunk *chunk, Vector3 cpos)
{
	chunk->position = cpos;
	chunk->block_data = calloc(CHUNK_BLOCK_COUNT, sizeof(u16));
	Vector3 wpos = Vector3Multiply(cpos, CHUNK_DIM);

	BLOCKS_ZYX_LOOP(CHUNK_WIDTH, CHUNK_HEIGHT, CHUNK_WIDTH)
	{
		determine_block(chunk, wpos, x, y, z);
	}
	mesh_chunk(chunk);
}

void fill_chunk_positions(Vector3 *positions, u32 size, ChunkmapKV **chunkmap)
{
	for (u32 i = 0; i < size; i++) {
		Chunk chunk;
		init_chunk(&chunk, positions[i]);
		hmput(*chunkmap, positions[i], chunk);
	}
	printf("hmlen is %d!\n", (i32)hmlen(*chunkmap));
}

void mesh_chunk(Chunk *chunk)
{
	Mesh mesh = { 0 };
	mesh.triangleCount = 1;
	mesh.vertexCount = mesh.triangleCount * 3;
	mesh.vertices = malloc(mesh.vertexCount * sizeof(float) * 3);
	mesh.texcoords = malloc(mesh.vertexCount * sizeof(float) * 2);
	mesh.normals = malloc(mesh.vertexCount * sizeof(float) * 3);

	mesh.vertices[0] = 0;
	mesh.vertices[1] = 0;
	mesh.vertices[2] = 0;
	mesh.normals[0] = 0;
	mesh.normals[1] = 1;
	mesh.normals[2] = 0;
	mesh.texcoords[0] = 0;
	mesh.texcoords[1] = 0;

	mesh.vertices[3] = 1;
	mesh.vertices[4] = 1;
	mesh.vertices[5] = 0;
	mesh.normals[3] = 0;
	mesh.normals[4] = 1;
	mesh.normals[5] = 0;
	mesh.texcoords[2] = 0.5f;
	mesh.texcoords[3] = 1.0f;

	mesh.vertices[6] = 1;
	mesh.vertices[7] = 0;
	mesh.vertices[8] = 0;
	mesh.normals[6] = 0;
	mesh.normals[7] = 1;
	mesh.normals[8] = 0;
	mesh.texcoords[4] = 1;
	mesh.texcoords[5] = 0;

	UploadMesh(&mesh, false);
	chunk->mesh = mesh;
}

void draw_chunk(Chunk *chunk)
{
	Vector3 wpos = chunk_world_position(chunk->position);
	/* BLOCKS_ZYX_LOOP(CHUNK_WIDTH, CHUNK_HEIGHT, CHUNK_WIDTH)
	{
		u16 block = chunk->block_data[block_index(x, y, z)];
		if (block == 0) continue;
		Vector3 drawposition = Vector3Add(wpos, (Vector3){ x, y, z });
		DrawCubeV(drawposition, Vector3One(), GREEN);
		DrawCubeWiresV(drawposition, Vector3One(), DARKGREEN);
	} */

	DrawCubeWiresV(Vector3Add(wpos, Vector3Multiply(Vector3One(), Vector3Scale(CHUNK_DIM, 0.5))),
		       Vector3Multiply(Vector3One(), CHUNK_DIM), BLUE);

	DrawMesh(chunk->mesh, LoadMaterialDefault(), MatrixTranslate(wpos.x, wpos.y, wpos.z));
}

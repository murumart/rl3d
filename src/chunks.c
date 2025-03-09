#include "chunks.h"

#include "ext/raylib.h"
#include "ext/raymath.h"
#include "ext/stb_ds.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>

typedef struct mesh_gen_info {
	float *vertices;
	u32 vertex_offset;
	u32 vertex_max;
	float *texcoords;
	u32 texcoord_offset;
	u32 texcoord_max;
	float *normals;
	u32 normal_offset;
	u32 normal_max;
} MeshGenInfo;

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

// does not check memory bounds!!
static inline void mesh_vertex(MeshGenInfo *info, Vector3 pos, Vector3 normal, Vector2 texcoord)
{
	info->vertices[info->vertex_offset + 0] = pos.x;
	info->vertices[info->vertex_offset + 1] = pos.y;
	info->vertices[info->vertex_offset + 2] = pos.z;
	info->vertex_offset += 3;

	info->normals[info->normal_offset + 0] = normal.x;
	info->normals[info->normal_offset + 1] = normal.y;
	info->normals[info->normal_offset + 2] = normal.z;
	info->normal_offset += 3;

	info->texcoords[info->texcoord_offset + 0] = texcoord.x;
	info->texcoords[info->texcoord_offset + 1] = texcoord.y;
	info->texcoord_offset += 2;
}

static void mesh_north_face(MeshGenInfo *info, u32 x, u32 y, u32 z)
{
	assert(info->vertex_offset + 18 <= info->vertex_max);
	assert(info->normal_offset + 18 <= info->normal_max);
	assert(info->texcoord_offset + 12 <= info->texcoord_max);

	mesh_vertex(info, (Vector3){ 0 + x, 0 + y, 0 + z }, (Vector3){ 0, 0, -1 }, (Vector2){ 0, 0 });
	mesh_vertex(info, (Vector3){ 1 + x, 1 + y, 0 + z }, (Vector3){ 0, 0, -1 }, (Vector2){ 0, 0 });
	mesh_vertex(info, (Vector3){ 1 + x, 0 + y, 0 + z }, (Vector3){ 0, 0, -1 }, (Vector2){ 0, 0 });
	mesh_vertex(info, (Vector3){ 0 + x, 0 + y, 0 + z }, (Vector3){ 0, 0, -1 }, (Vector2){ 0, 0 });
	mesh_vertex(info, (Vector3){ 0 + x, 1 + y, 0 + z }, (Vector3){ 0, 0, -1 }, (Vector2){ 0, 0 });
	mesh_vertex(info, (Vector3){ 1 + x, 1 + y, 0 + z }, (Vector3){ 0, 0, -1 }, (Vector2){ 0, 0 });
}

static void mesh_south_face(MeshGenInfo *info, u32 x, u32 y, u32 z)
{
	assert(info->vertex_offset + 18 <= info->vertex_max);
	assert(info->normal_offset + 18 <= info->normal_max);
	assert(info->texcoord_offset + 12 <= info->texcoord_max);

	mesh_vertex(info, (Vector3){ 1 + x, 0 + y, 1 + z }, (Vector3){ 0, 0, 1 }, (Vector2){ 0, 0 });
	mesh_vertex(info, (Vector3){ 0 + x, 1 + y, 1 + z }, (Vector3){ 0, 0, 1 }, (Vector2){ 0, 0 });
	mesh_vertex(info, (Vector3){ 0 + x, 0 + y, 1 + z }, (Vector3){ 0, 0, 1 }, (Vector2){ 0, 0 });
	mesh_vertex(info, (Vector3){ 1 + x, 0 + y, 1 + z }, (Vector3){ 0, 0, 1 }, (Vector2){ 0, 0 });
	mesh_vertex(info, (Vector3){ 1 + x, 1 + y, 1 + z }, (Vector3){ 0, 0, 1 }, (Vector2){ 0, 0 });
	mesh_vertex(info, (Vector3){ 0 + x, 1 + y, 1 + z }, (Vector3){ 0, 0, 1 }, (Vector2){ 0, 0 });
}

static void mesh_east_face(MeshGenInfo *info, u32 x, u32 y, u32 z)
{
	assert(info->vertex_offset + 18 <= info->vertex_max);
	assert(info->normal_offset + 18 <= info->normal_max);
	assert(info->texcoord_offset + 12 <= info->texcoord_max);

	mesh_vertex(info, (Vector3){ 1 + x, 0 + y, 0 + z }, (Vector3){ 1, 0, 0 }, (Vector2){ 0, 0 });
	mesh_vertex(info, (Vector3){ 1 + x, 1 + y, 0 + z }, (Vector3){ 1, 0, 0 }, (Vector2){ 0, 0 });
	mesh_vertex(info, (Vector3){ 1 + x, 0 + y, 1 + z }, (Vector3){ 1, 0, 0 }, (Vector2){ 0, 0 });
	mesh_vertex(info, (Vector3){ 1 + x, 1 + y, 0 + z }, (Vector3){ 1, 0, 0 }, (Vector2){ 0, 0 });
	mesh_vertex(info, (Vector3){ 1 + x, 1 + y, 1 + z }, (Vector3){ 1, 0, 0 }, (Vector2){ 0, 0 });
	mesh_vertex(info, (Vector3){ 1 + x, 0 + y, 1 + z }, (Vector3){ 1, 0, 0 }, (Vector2){ 0, 0 });
}

static void mesh_west_face(MeshGenInfo *info, u32 x, u32 y, u32 z)
{
	assert(info->vertex_offset + 18 <= info->vertex_max);
	assert(info->normal_offset + 18 <= info->normal_max);
	assert(info->texcoord_offset + 12 <= info->texcoord_max);

	mesh_vertex(info, (Vector3){ 0 + x, 0 + y, 1 + z }, (Vector3){ 1, 0, 0 }, (Vector2){ 0, 0 });
	mesh_vertex(info, (Vector3){ 0 + x, 1 + y, 0 + z }, (Vector3){ 1, 0, 0 }, (Vector2){ 0, 0 });
	mesh_vertex(info, (Vector3){ 0 + x, 0 + y, 0 + z }, (Vector3){ 1, 0, 0 }, (Vector2){ 0, 0 });
	mesh_vertex(info, (Vector3){ 0 + x, 0 + y, 1 + z }, (Vector3){ 1, 0, 0 }, (Vector2){ 0, 0 });
	mesh_vertex(info, (Vector3){ 0 + x, 1 + y, 1 + z }, (Vector3){ 1, 0, 0 }, (Vector2){ 0, 0 });
	mesh_vertex(info, (Vector3){ 0 + x, 1 + y, 0 + z }, (Vector3){ 1, 0, 0 }, (Vector2){ 0, 0 });
}

static void mesh_top_face(MeshGenInfo *info, u32 x, u32 y, u32 z)
{
	assert(info->vertex_offset + 18 <= info->vertex_max);
	assert(info->normal_offset + 18 <= info->normal_max);
	assert(info->texcoord_offset + 12 <= info->texcoord_max);

	mesh_vertex(info, (Vector3){ 0 + x, 1 + y, 0 + z }, (Vector3){ 1, 0, 0 }, (Vector2){ 0, 0 });
	mesh_vertex(info, (Vector3){ 1 + x, 1 + y, 1 + z }, (Vector3){ 1, 0, 0 }, (Vector2){ 0, 0 });
	mesh_vertex(info, (Vector3){ 1 + x, 1 + y, 0 + z }, (Vector3){ 1, 0, 0 }, (Vector2){ 0, 0 });
	mesh_vertex(info, (Vector3){ 0 + x, 1 + y, 0 + z }, (Vector3){ 1, 0, 0 }, (Vector2){ 0, 0 });
	mesh_vertex(info, (Vector3){ 0 + x, 1 + y, 1 + z }, (Vector3){ 1, 0, 0 }, (Vector2){ 0, 0 });
	mesh_vertex(info, (Vector3){ 1 + x, 1 + y, 1 + z }, (Vector3){ 1, 0, 0 }, (Vector2){ 0, 0 });
}

static void mesh_bottom_face(MeshGenInfo *info, u32 x, u32 y, u32 z)
{
	assert(info->vertex_offset + 18 <= info->vertex_max);
	assert(info->normal_offset + 18 <= info->normal_max);
	assert(info->texcoord_offset + 12 <= info->texcoord_max);

	mesh_vertex(info, (Vector3){ 0 + x, 0 + y, 0 + z }, (Vector3){ 1, 0, 0 }, (Vector2){ 0, 0 });
	mesh_vertex(info, (Vector3){ 1 + x, 0 + y, 0 + z }, (Vector3){ 1, 0, 0 }, (Vector2){ 0, 0 });
	mesh_vertex(info, (Vector3){ 0 + x, 0 + y, 1 + z }, (Vector3){ 1, 0, 0 }, (Vector2){ 0, 0 });
	mesh_vertex(info, (Vector3){ 0 + x, 0 + y, 1 + z }, (Vector3){ 1, 0, 0 }, (Vector2){ 0, 0 });
	mesh_vertex(info, (Vector3){ 1 + x, 0 + y, 0 + z }, (Vector3){ 1, 0, 0 }, (Vector2){ 0, 0 });
	mesh_vertex(info, (Vector3){ 1 + x, 0 + y, 1 + z }, (Vector3){ 1, 0, 0 }, (Vector2){ 0, 0 });
}

void mesh_chunk(Chunk *chunk)
{
	Mesh mesh = { 0 };
	// 2 triangles per face, 6 faces per block... how many blocks per chunk?
	u32 blocks = 0;
	BLOCKS_ZYX_LOOP(CHUNK_WIDTH, CHUNK_HEIGHT, CHUNK_WIDTH)
	{
		if (chunk->block_data[block_index(x, y, z)] != 0) blocks += 1; 
	}

	mesh.triangleCount = 2 * 6 * blocks;
	mesh.vertexCount = mesh.triangleCount * 3;

	MeshGenInfo info = {
		.vertex_max = mesh.vertexCount * 3,
		.normal_max = mesh.vertexCount * 3,
		.texcoord_max = mesh.vertexCount * 2,
	};

	info.vertices = malloc(info.vertex_max * sizeof(float));
	info.normals = malloc(info.normal_max * sizeof(float));
	info.texcoords = malloc(info.texcoord_max * sizeof(float));

	BLOCKS_ZYX_LOOP(CHUNK_WIDTH, CHUNK_HEIGHT, CHUNK_WIDTH)
	{
		if (chunk->block_data[block_index(x, y, z)] == 0) continue;
		mesh_north_face(&info, x, y, z);
		mesh_south_face(&info, x, y, z);
		mesh_east_face(&info, x, y, z);
		mesh_west_face(&info, x, y, z);
		mesh_top_face(&info, x, y, z);
		mesh_bottom_face(&info, x, y, z);
	}

	mesh.vertices = info.vertices;
	mesh.normals = info.normals;
	mesh.texcoords = info.texcoords;

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

void process_chunk(Chunk *chunk, float delta)
{
}

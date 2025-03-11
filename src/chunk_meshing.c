#include "chunk_meshing.h"

#include "world.h"

#include "ext/raylib.h"
#include "ext/raymath.h"

#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

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

	info->texcoords[info->texcoord_offset + 0] = texcoord.x / 16;
	info->texcoords[info->texcoord_offset + 1] = texcoord.y / 16;
	info->texcoord_offset += 2;
}

static void mesh_north_face(MeshGenInfo *info, u32 x, u32 y, u32 z)
{
	assert(info->vertex_offset + 18 <= info->vertex_max);
	assert(info->normal_offset + 18 <= info->normal_max);
	assert(info->texcoord_offset + 12 <= info->texcoord_max);

	mesh_vertex(info, (Vector3){ 0 + x, 0 + y, 0 + z }, (Vector3){ 0, 0, -1 }, (Vector2){ 1, 1 });
	mesh_vertex(info, (Vector3){ 1 + x, 1 + y, 0 + z }, (Vector3){ 0, 0, -1 }, (Vector2){ 0, 0 });
	mesh_vertex(info, (Vector3){ 1 + x, 0 + y, 0 + z }, (Vector3){ 0, 0, -1 }, (Vector2){ 0, 1 });
	mesh_vertex(info, (Vector3){ 0 + x, 0 + y, 0 + z }, (Vector3){ 0, 0, -1 }, (Vector2){ 1, 1 });
	mesh_vertex(info, (Vector3){ 0 + x, 1 + y, 0 + z }, (Vector3){ 0, 0, -1 }, (Vector2){ 1, 0 });
	mesh_vertex(info, (Vector3){ 1 + x, 1 + y, 0 + z }, (Vector3){ 0, 0, -1 }, (Vector2){ 0, 0 });
}

static void mesh_south_face(MeshGenInfo *info, u32 x, u32 y, u32 z)
{
	assert(info->vertex_offset + 18 <= info->vertex_max);
	assert(info->normal_offset + 18 <= info->normal_max);
	assert(info->texcoord_offset + 12 <= info->texcoord_max);

	mesh_vertex(info, (Vector3){ 1 + x, 0 + y, 1 + z }, (Vector3){ 0, 0, 1 }, (Vector2){ 1, 1 });
	mesh_vertex(info, (Vector3){ 0 + x, 1 + y, 1 + z }, (Vector3){ 0, 0, 1 }, (Vector2){ 0, 0 });
	mesh_vertex(info, (Vector3){ 0 + x, 0 + y, 1 + z }, (Vector3){ 0, 0, 1 }, (Vector2){ 0, 1 });
	mesh_vertex(info, (Vector3){ 1 + x, 0 + y, 1 + z }, (Vector3){ 0, 0, 1 }, (Vector2){ 1, 1 });
	mesh_vertex(info, (Vector3){ 1 + x, 1 + y, 1 + z }, (Vector3){ 0, 0, 1 }, (Vector2){ 1, 0 });
	mesh_vertex(info, (Vector3){ 0 + x, 1 + y, 1 + z }, (Vector3){ 0, 0, 1 }, (Vector2){ 0, 0 });
}

static void mesh_east_face(MeshGenInfo *info, u32 x, u32 y, u32 z)
{
	assert(info->vertex_offset + 18 <= info->vertex_max);
	assert(info->normal_offset + 18 <= info->normal_max);
	assert(info->texcoord_offset + 12 <= info->texcoord_max);

	mesh_vertex(info, (Vector3){ 1 + x, 0 + y, 0 + z }, (Vector3){ 1, 0, 0 }, (Vector2){ 0, 1 });
	mesh_vertex(info, (Vector3){ 1 + x, 1 + y, 0 + z }, (Vector3){ 1, 0, 0 }, (Vector2){ 0, 0 });
	mesh_vertex(info, (Vector3){ 1 + x, 0 + y, 1 + z }, (Vector3){ 1, 0, 0 }, (Vector2){ 1, 1 });
	mesh_vertex(info, (Vector3){ 1 + x, 1 + y, 0 + z }, (Vector3){ 1, 0, 0 }, (Vector2){ 0, 0 });
	mesh_vertex(info, (Vector3){ 1 + x, 1 + y, 1 + z }, (Vector3){ 1, 0, 0 }, (Vector2){ 1, 0 });
	mesh_vertex(info, (Vector3){ 1 + x, 0 + y, 1 + z }, (Vector3){ 1, 0, 0 }, (Vector2){ 1, 1 });
}

static void mesh_west_face(MeshGenInfo *info, u32 x, u32 y, u32 z)
{
	assert(info->vertex_offset + 18 <= info->vertex_max);
	assert(info->normal_offset + 18 <= info->normal_max);
	assert(info->texcoord_offset + 12 <= info->texcoord_max);

	mesh_vertex(info, (Vector3){ 0 + x, 0 + y, 1 + z }, (Vector3){ -1, 0, 0 }, (Vector2){ 1, 1 });
	mesh_vertex(info, (Vector3){ 0 + x, 1 + y, 0 + z }, (Vector3){ -1, 0, 0 }, (Vector2){ 0, 0 });
	mesh_vertex(info, (Vector3){ 0 + x, 0 + y, 0 + z }, (Vector3){ -1, 0, 0 }, (Vector2){ 0, 1 });
	mesh_vertex(info, (Vector3){ 0 + x, 0 + y, 1 + z }, (Vector3){ -1, 0, 0 }, (Vector2){ 1, 1 });
	mesh_vertex(info, (Vector3){ 0 + x, 1 + y, 1 + z }, (Vector3){ -1, 0, 0 }, (Vector2){ 1, 0 });
	mesh_vertex(info, (Vector3){ 0 + x, 1 + y, 0 + z }, (Vector3){ -1, 0, 0 }, (Vector2){ 0, 0 });
}

static void mesh_top_face(MeshGenInfo *info, u32 x, u32 y, u32 z)
{
	assert(info->vertex_offset + 18 <= info->vertex_max);
	assert(info->normal_offset + 18 <= info->normal_max);
	assert(info->texcoord_offset + 12 <= info->texcoord_max);

	mesh_vertex(info, (Vector3){ 0 + x, 1 + y, 0 + z }, (Vector3){ 0, 1, 0 }, (Vector2){ 0, 0 });
	mesh_vertex(info, (Vector3){ 1 + x, 1 + y, 1 + z }, (Vector3){ 0, 1, 0 }, (Vector2){ 1, 1 });
	mesh_vertex(info, (Vector3){ 1 + x, 1 + y, 0 + z }, (Vector3){ 0, 1, 0 }, (Vector2){ 1, 0 });
	mesh_vertex(info, (Vector3){ 0 + x, 1 + y, 0 + z }, (Vector3){ 0, 1, 0 }, (Vector2){ 0, 0 });
	mesh_vertex(info, (Vector3){ 0 + x, 1 + y, 1 + z }, (Vector3){ 0, 1, 0 }, (Vector2){ 0, 1 });
	mesh_vertex(info, (Vector3){ 1 + x, 1 + y, 1 + z }, (Vector3){ 0, 1, 0 }, (Vector2){ 1, 1 });
}

static void mesh_bottom_face(MeshGenInfo *info, u32 x, u32 y, u32 z)
{
	assert(info->vertex_offset + 18 <= info->vertex_max);
	assert(info->normal_offset + 18 <= info->normal_max);
	assert(info->texcoord_offset + 12 <= info->texcoord_max);

	mesh_vertex(info, (Vector3){ 0 + x, 0 + y, 0 + z }, (Vector3){ 0, -1, 0 }, (Vector2){ 0, 0 });
	mesh_vertex(info, (Vector3){ 1 + x, 0 + y, 0 + z }, (Vector3){ 0, -1, 0 }, (Vector2){ 1, 0 });
	mesh_vertex(info, (Vector3){ 0 + x, 0 + y, 1 + z }, (Vector3){ 0, -1, 0 }, (Vector2){ 0, 1 });
	mesh_vertex(info, (Vector3){ 0 + x, 0 + y, 1 + z }, (Vector3){ 0, -1, 0 }, (Vector2){ 0, 1 });
	mesh_vertex(info, (Vector3){ 1 + x, 0 + y, 0 + z }, (Vector3){ 0, -1, 0 }, (Vector2){ 1, 0 });
	mesh_vertex(info, (Vector3){ 1 + x, 0 + y, 1 + z }, (Vector3){ 0, -1, 0 }, (Vector2){ 1, 1 });
}

static bool check_block_transp(Chunk *chunk, World *world, i32 x, i32 y, i32 z)
{
	if (x < 0 || x >= CHUNK_WIDTH || y < 0 || y >= CHUNK_HEIGHT || z < 0 || z >= CHUNK_WIDTH) {
		// xyz are relative to the passed in chunk
		BlockPosition bpos = { x, y, z };
		ChunkPosition outside_location = chunkpos_from_blockpos(bpos);
		// add that chunk's position so the location becomes global
		outside_location.x += chunk->position.x;
		outside_location.y += chunk->position.y;
		outside_location.z += chunk->position.z;
		Chunk *outside_chunk = world_get_chunk(world, outside_location);
		if (outside_chunk == NULL) return true;

		BlockPosition inside = chunk_world_bpos_to_local(bpos);
		return outside_chunk->block_data[block_indexp(inside)] == 0;
	}
	return chunk->block_data[block_index((u32)x, (u32)y, (u32)z)] == 0;
}

void mesh_chunk(Chunk *chunk, World *world)
{
	Mesh mesh = { 0 };
	//BlockPosition cwpos = blockpos_from_chunkpos(chunk->position);

	// 2 triangles per face, how many faces per chunk?
	u32 faces = 0;
	u32 blocks = 0;
	BLOCKS_ZYX_LOOP(CHUNK_WIDTH, CHUNK_HEIGHT, CHUNK_WIDTH)
	{
		if (chunk->block_data[block_index(x, y, z)] == 0) continue;
		else blocks++;

		if (check_block_transp(chunk, world, x, y, z - 1)) faces++;
		if (check_block_transp(chunk, world, x, y, z + 1)) faces++;
		if (check_block_transp(chunk, world, x + 1, y, z)) faces++;
		if (check_block_transp(chunk, world, x - 1, y, z)) faces++;
		if (check_block_transp(chunk, world, x, y + 1, z)) faces++;
		if (check_block_transp(chunk, world, x, y - 1, z)) faces++;
	}

	mesh.triangleCount = 2 * faces;
	//mesh.triangleCount = 2 * 6 * blocks;
	mesh.vertexCount = mesh.triangleCount * 3;

	MeshGenInfo info = {
		.vertex_max = mesh.vertexCount * 3,
		.normal_max = mesh.vertexCount * 3,
		.texcoord_max = mesh.vertexCount * 2,
	};

	info.vertices = malloc(info.vertex_max * sizeof(float));
	info.normals = malloc(info.normal_max * sizeof(float));
	info.texcoords = malloc(info.texcoord_max * sizeof(float));

	// TODO: binary meshing or whatever
	BLOCKS_ZYX_LOOP(CHUNK_WIDTH, CHUNK_HEIGHT, CHUNK_WIDTH)
	{
		if (chunk->block_data[block_index(x, y, z)] == 0) continue;

		if (check_block_transp(chunk, world, x, y, z - 1)) mesh_north_face(&info, x, y, z);
		if (check_block_transp(chunk, world, x, y, z + 1)) mesh_south_face(&info, x, y, z);
		if (check_block_transp(chunk, world, x + 1, y, z)) mesh_east_face(&info, x, y, z);
		if (check_block_transp(chunk, world, x - 1, y, z)) mesh_west_face(&info, x, y, z);
		if (check_block_transp(chunk, world, x, y + 1, z)) mesh_top_face(&info, x, y, z);
		if (check_block_transp(chunk, world, x, y - 1, z)) mesh_bottom_face(&info, x, y, z);
	}

	mesh.vertices = info.vertices;
	mesh.normals = info.normals;
	mesh.texcoords = info.texcoords;

	UploadMesh(&mesh, false);
	chunk->mesh = mesh;
}
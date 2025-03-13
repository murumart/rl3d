#include "chunk_meshing.h"

#include "world.h"

#include "ext/raylib.h"
#include "ext/raymath.h"

#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>

#define VERTICES_PER_FACE 4
#define INDICES_PER_FACE 6

typedef struct mesh_gen_info {
	u16 *indices;
	u32 index_offset;
	u32 index_max;
	float *vertex_coords;
	u32 vertex_coord_offset;
	u32 vertex_coord_max;
	float *uv_coords;
	u32 uv_offset;
	u32 uv_max;
	float *normal_sides;
	u32 normal_side_offset;
	u32 normal_side_max;
} MeshGenInfo;

typedef struct six_indices {
	u16 a, b, c, d, e, f;
} SixIndices;

// does not check memory bounds!!
static inline void mesh_vertex(MeshGenInfo *info, Vector3 pos, Vector3 normal, Vector2 texcoord)
{
	info->vertex_coords[info->vertex_coord_offset + 0] = pos.x;
	info->vertex_coords[info->vertex_coord_offset + 1] = pos.y;
	info->vertex_coords[info->vertex_coord_offset + 2] = pos.z;
	info->vertex_coord_offset += 3;

	info->normal_sides[info->normal_side_offset + 0] = normal.x;
	info->normal_sides[info->normal_side_offset + 1] = normal.y;
	info->normal_sides[info->normal_side_offset + 2] = normal.z;
	info->normal_side_offset += 3;

	info->uv_coords[info->uv_offset + 0] = texcoord.x / 16;
	info->uv_coords[info->uv_offset + 1] = texcoord.y / 16;
	info->uv_offset += 2;
}

static inline void index_vertices(MeshGenInfo *info, u16 a, u16 b, u16 c, u16 d, u16 e, u16 f)
{
	// subtract 4 from the offset because you add 4 vertices each time and to index them
	// you gotta go backward
	u32 vertex_offset = info->vertex_coord_offset / 3 - VERTICES_PER_FACE;
	info->indices[info->index_offset + 0] = a + vertex_offset;
	info->indices[info->index_offset + 1] = b + vertex_offset;
	info->indices[info->index_offset + 2] = c + vertex_offset;
	info->indices[info->index_offset + 3] = d + vertex_offset;
	info->indices[info->index_offset + 4] = e + vertex_offset;
	info->indices[info->index_offset + 5] = f + vertex_offset;
	info->index_offset += 6;
}

static inline void assert_mesh_gen_space_has_room(MeshGenInfo *info)
{
	assert(info->vertex_coord_offset + 3 * VERTICES_PER_FACE <= info->vertex_coord_max);
	assert(info->normal_side_offset + 3 * VERTICES_PER_FACE <= info->normal_side_max);
	assert(info->uv_offset + 2 * VERTICES_PER_FACE <= info->uv_max);
	assert(info->index_offset + INDICES_PER_FACE <= info->index_max);
}

static void mesh_north_face(MeshGenInfo *info, u32 x, u32 y, u32 z)
{
	assert_mesh_gen_space_has_room(info);

	mesh_vertex(info, (Vector3){ 0 + x, 0 + y, 0 + z }, (Vector3){ 0, 0, -1 }, (Vector2){ 1, 1 });
	mesh_vertex(info, (Vector3){ 1 + x, 1 + y, 0 + z }, (Vector3){ 0, 0, -1 }, (Vector2){ 0, 0 });
	mesh_vertex(info, (Vector3){ 1 + x, 0 + y, 0 + z }, (Vector3){ 0, 0, -1 }, (Vector2){ 0, 1 });
	mesh_vertex(info, (Vector3){ 0 + x, 1 + y, 0 + z }, (Vector3){ 0, 0, -1 }, (Vector2){ 1, 0 });
	index_vertices(info, 0, 1, 2, 3, 1, 0);
}

static void mesh_south_face(MeshGenInfo *info, u32 x, u32 y, u32 z)
{
	assert_mesh_gen_space_has_room(info);

	mesh_vertex(info, (Vector3){ 1 + x, 0 + y, 1 + z }, (Vector3){ 0, 0, 1 }, (Vector2){ 1, 1 });
	mesh_vertex(info, (Vector3){ 0 + x, 1 + y, 1 + z }, (Vector3){ 0, 0, 1 }, (Vector2){ 0, 0 });
	mesh_vertex(info, (Vector3){ 0 + x, 0 + y, 1 + z }, (Vector3){ 0, 0, 1 }, (Vector2){ 0, 1 });
	mesh_vertex(info, (Vector3){ 1 + x, 1 + y, 1 + z }, (Vector3){ 0, 0, 1 }, (Vector2){ 1, 0 });
	index_vertices(info, 0, 1, 2, 3, 1, 0);
}

static void mesh_east_face(MeshGenInfo *info, u32 x, u32 y, u32 z)
{
	assert_mesh_gen_space_has_room(info);

	mesh_vertex(info, (Vector3){ 1 + x, 0 + y, 0 + z }, (Vector3){ 1, 0, 0 }, (Vector2){ 0, 1 });
	mesh_vertex(info, (Vector3){ 1 + x, 1 + y, 1 + z }, (Vector3){ 1, 0, 0 }, (Vector2){ 1, 0 });
	mesh_vertex(info, (Vector3){ 1 + x, 0 + y, 1 + z }, (Vector3){ 1, 0, 0 }, (Vector2){ 1, 1 });
	mesh_vertex(info, (Vector3){ 1 + x, 1 + y, 0 + z }, (Vector3){ 1, 0, 0 }, (Vector2){ 0, 0 });
	index_vertices(info, 0, 1, 2, 3, 1, 0);
}

static void mesh_west_face(MeshGenInfo *info, u32 x, u32 y, u32 z)
{
	assert_mesh_gen_space_has_room(info);

	mesh_vertex(info, (Vector3){ 0 + x, 0 + y, 1 + z }, (Vector3){ -1, 0, 0 }, (Vector2){ 1, 1 });
	mesh_vertex(info, (Vector3){ 0 + x, 1 + y, 0 + z }, (Vector3){ -1, 0, 0 }, (Vector2){ 0, 0 });
	mesh_vertex(info, (Vector3){ 0 + x, 0 + y, 0 + z }, (Vector3){ -1, 0, 0 }, (Vector2){ 0, 1 });
	mesh_vertex(info, (Vector3){ 0 + x, 1 + y, 1 + z }, (Vector3){ -1, 0, 0 }, (Vector2){ 1, 0 });
	index_vertices(info, 0, 1, 2, 3, 1, 0);
}

static void mesh_top_face(MeshGenInfo *info, u32 x, u32 y, u32 z)
{
	assert_mesh_gen_space_has_room(info);

	mesh_vertex(info, (Vector3){ 0 + x, 1 + y, 0 + z }, (Vector3){ 0, 1, 0 }, (Vector2){ 0, 0 });
	mesh_vertex(info, (Vector3){ 1 + x, 1 + y, 1 + z }, (Vector3){ 0, 1, 0 }, (Vector2){ 1, 1 });
	mesh_vertex(info, (Vector3){ 1 + x, 1 + y, 0 + z }, (Vector3){ 0, 1, 0 }, (Vector2){ 1, 0 });
	mesh_vertex(info, (Vector3){ 0 + x, 1 + y, 1 + z }, (Vector3){ 0, 1, 0 }, (Vector2){ 0, 1 });
	index_vertices(info, 0, 1, 2, 3, 1, 0);
}

static void mesh_bottom_face(MeshGenInfo *info, u32 x, u32 y, u32 z)
{
	assert_mesh_gen_space_has_room(info);

	mesh_vertex(info, (Vector3){ 0 + x, 0 + y, 0 + z }, (Vector3){ 0, -1, 0 }, (Vector2){ 1, 1 });
	mesh_vertex(info, (Vector3){ 1 + x, 0 + y, 1 + z }, (Vector3){ 0, -1, 0 }, (Vector2){ 0, 0 });
	mesh_vertex(info, (Vector3){ 0 + x, 0 + y, 1 + z }, (Vector3){ 0, -1, 0 }, (Vector2){ 1, 0 });
	mesh_vertex(info, (Vector3){ 1 + x, 0 + y, 0 + z }, (Vector3){ 0, -1, 0 }, (Vector2){ 0, 1 });
	index_vertices(info, 0, 1, 2, 3, 1, 0);
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
		blocks++;

		if (check_block_transp(chunk, world, x, y, z - 1)) faces++;
		if (check_block_transp(chunk, world, x, y, z + 1)) faces++;
		if (check_block_transp(chunk, world, x + 1, y, z)) faces++;
		if (check_block_transp(chunk, world, x - 1, y, z)) faces++;
		if (check_block_transp(chunk, world, x, y + 1, z)) faces++;
		if (check_block_transp(chunk, world, x, y - 1, z)) faces++;
	}

	mesh.triangleCount = 2 * faces;
	//mesh.triangleCount = 2 * 6 * blocks;
	mesh.vertexCount = VERTICES_PER_FACE * faces;

	MeshGenInfo info = {
		.vertex_coord_max = mesh.vertexCount * 3, // each vertex is has a 3 float vector position
		.normal_side_max = mesh.vertexCount * 3, // normal vector (3 floats) for each vertex
		.uv_max = mesh.vertexCount * 2, // 2 floats for texture coordinates for each vertex
		.index_max = INDICES_PER_FACE * faces,
	};

	assert(info.vertex_coord_max / 3 < UINT16_MAX);

	info.vertex_coords = malloc(info.vertex_coord_max * sizeof(float));
	memset(info.vertex_coords, 0, sizeof(float) * info.vertex_coord_max);
	info.normal_sides = malloc(info.normal_side_max * sizeof(float));
	info.uv_coords = malloc(info.uv_max * sizeof(float));
	info.indices = malloc(info.index_max * sizeof(u16));
	memset(info.indices, 0, sizeof(u16) * info.index_max);

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

	mesh.vertices = info.vertex_coords;
	mesh.normals = info.normal_sides;
	mesh.texcoords = info.uv_coords;
	mesh.indices = info.indices;

	UploadMesh(&mesh, false);
	chunk->mesh = mesh;
}
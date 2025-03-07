#ifndef __CHUNKS_H__
#define __CHUNKS_H__

#include "lang.h"

#include "ext/raylib.h"

#define BLOCKS_ZYX_LOOP(X, Y, Z)            \
	for (u32 z = 0; z < Z; z++)         \
		for (u32 y = 0; y < Y; y++) \
			for (u32 x = 0; x < X; x++)

static const u32 CHUNK_WIDTH = 16;
static const u32 CHUNK_HEIGHT = 16;
static const u32 CHUNK_BLOCK_COUNT = CHUNK_WIDTH * CHUNK_HEIGHT * CHUNK_WIDTH;
static const Vector3 CHUNK_DIM = { CHUNK_WIDTH, CHUNK_HEIGHT, CHUNK_WIDTH };

typedef struct chunk {
	u16 *block_data;
	Mesh mesh;
	Vector3 position;
} Chunk;

typedef struct chunkmap_kv {
	Vector3 key;
	Chunk value;
} ChunkmapKV;

u32 block_index(u32 x, u32 y, u32 z);
Vector3 chunk_world_position(Vector3 chunkpos);
void init_chunk(Chunk *chunk, Vector3 cpos);
void fill_chunk_positions(Vector3 *positions, u32 size, ChunkmapKV **chunkmap);
void mesh_chunk(Chunk *chunk);
void draw_chunk(Chunk *chunk);

#endif // __CHUNKS_H__
#ifndef __CHUNKS_H__
#define __CHUNKS_H__

#include "lang.h"

#include "raylib.h"

static const u32 CHUNK_WIDTH = 16;
static const u32 CHUNK_HEIGHT = 16;
static const u32 CHUNK_BLOCK_COUNT = CHUNK_WIDTH * CHUNK_HEIGHT * CHUNK_WIDTH;
static const Vector3 CHUNK_DIM = { CHUNK_WIDTH, CHUNK_HEIGHT, CHUNK_WIDTH };

typedef struct chunk {
	u16 *block_data;
	Vector3 position;
} Chunk;

typedef struct chunkmap_kv {
	Vector3 key;
	Chunk value;
} ChunkmapKV;

u32 block_index(u32 x, u32 y, u32 z);
Vector3 chunk_world_position(Vector3 chunkpos);
void init_chunk(Chunk *chunk);
void fill_chunk_positions(Vector3 *positions, u32 size, ChunkmapKV **chunkmap);
void draw_chunk(Chunk *chunk);

#endif // __CHUNKS_H__
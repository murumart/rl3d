#ifndef __CHUNKS_H__
#define __CHUNKS_H__

#include "lang.h"

#include "ext/raylib.h"

// loop through blocks in z y x order. pass in dimensions of chunk.
// provides x, y, z (u32) values for following code block.
#define BLOCKS_ZYX_LOOP(X, Y, Z)            \
	for (u32 z = 0; z < Z; z++)         \
		for (u32 y = 0; y < Y; y++) \
			for (u32 x = 0; x < X; x++)

static const u32 CHUNK_WIDTH = 16;
static const u32 CHUNK_HEIGHT = 16;
static const u32 CHUNK_BLOCK_COUNT = CHUNK_WIDTH * CHUNK_HEIGHT * CHUNK_WIDTH;
static const Vector3 CHUNK_DIM = { CHUNK_WIDTH, CHUNK_HEIGHT, CHUNK_WIDTH };

typedef struct chunk_position {
	i32 x;
	i32 y;
	i32 z;
} ChunkPosition;

typedef struct block_position {
	i32 x;
	i32 y;
	i32 z;
} BlockPosition;

typedef struct chunk {
	u16 *block_data;
	Mesh mesh;
	ChunkPosition position;
} Chunk;

typedef struct chunkmap_kv {
	ChunkPosition key;
	Chunk value;
} ChunkmapKV;

u32 block_index(u32 x, u32 y, u32 z);
u32 block_indexp(BlockPosition bpos);
BlockPosition chunkpos_to_blockpos(ChunkPosition cpos);
void init_chunk(Chunk *chunk, ChunkPosition cpos);
void fill_chunk_positions(ChunkPosition *positions, u32 size, ChunkmapKV **chunkmap);

void draw_chunk(Chunk *chunk, Material material);
void process_chunk(Chunk *chunk, float delta);

#endif // __CHUNKS_H__
#ifndef __CHUNKS_H__
#define __CHUNKS_H__

#include "lang.h"

#include "world.h"

#include "ext/raylib.h"

// loop through blocks in z y x order. pass in dimensions of chunk.
// provides x, y, z (u32) values for following code block.
#define BLOCKS_ZYX_LOOP(X, Y, Z)            \
	for (u32 z = 0; z < Z; z++)         \
		for (u32 y = 0; y < Y; y++) \
			for (u32 x = 0; x < X; x++)

#define CHUNK_WIDTH 16U
#define CHUNK_HEIGHT 16U

#define CHUNK_FLAG_MESH_CURRENT 0x1u

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
	World *world;
	u32 flags;
} Chunk;

// stores a chunk as a value and its position (of value ChunkPosition) as a key
// used in the stb_ds hashmap to store chunks
typedef struct chunkmap_kv {
	ChunkPosition key;
	Chunk value;
} ChunkmapKV;

u32 block_index(u32 x, u32 y, u32 z);
u32 block_indexp(BlockPosition bpos);
BlockPosition blockpos_from_chunkpos(ChunkPosition cpos);
ChunkPosition chunkpos_from_blockpos(BlockPosition bpos);
ChunkPosition chunkpos_from_worldpos(Vector3 wpos);
BlockPosition chunk_world_bpos_to_local(BlockPosition global);
bool are_chunkposes_equal(const ChunkPosition a, const ChunkPosition b);

void init_chunk(Chunk *chunk, World *world, ChunkPosition cpos);

void chunks_meshing_process(World *world, float delta);
void chunk_draw(Chunk *chunk, Material material);
void chunk_process(Chunk *chunk, float delta);

#endif // __CHUNKS_H__
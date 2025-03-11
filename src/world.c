#include "world.h"
#include "chunks.h"

#include "lang.h"
#include "ext/stb_ds.h"

#include <stdio.h>

void world_init(World *world)
{
	const i32 WDIMS = 4;
	ChunkPosition *initchunks = NULL;
	for (i32 x = -WDIMS * 2; x < WDIMS * 2; x++)
		for (i32 y = -WDIMS; y < WDIMS; y++)
			for (i32 z = -WDIMS * 2; z < WDIMS * 2; z++) {
				ChunkPosition pos = { x, y, z };
				arrput(initchunks, pos);
			}

	world->chunkmap = NULL;
	world_fill_chunk_positions(world, initchunks, arrlenu(initchunks));
	printf("hmlen is %d!\n", (i32)hmlen(world->chunkmap));
}

void world_fill_chunk_positions(World *world, ChunkPosition *positions, u32 size)
{
	for (u32 i = 0; i < size; i++) {
		Chunk chunk;
		init_chunk(&chunk, world, positions[i]);
		hmput(world->chunkmap, positions[i], chunk);
	}
	printf("hmlen is %d!\n", (i32)hmlen(world->chunkmap));
}

Chunk *world_get_chunk(World *world, ChunkPosition cpos)
{
	ChunkmapKV *kv = hmgetp_null(world->chunkmap, cpos);
	if (kv == NULL) return NULL;
	return &kv->value;
}
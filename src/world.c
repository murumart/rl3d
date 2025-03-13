#include "world.h"
#include "chunks.h"

#include "lang.h"
#include "ext/stb_ds.h"

#include <stdio.h>
#include <stdlib.h>

static i32 compare_cpos_distances_to_world_centre(const void *a, const void *b)
{
	const ChunkPosition *ca = a;
	const ChunkPosition *cb = b;
	i32 distsa = ca->x * ca->x + ca->y * ca->y + ca->z + ca->z;
	i32 distsb = cb->x * cb->x + cb->y * cb->y + cb->z + cb->z;
	return distsa - distsb;
}

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

	qsort(initchunks, arrlenu(initchunks), sizeof(ChunkPosition), compare_cpos_distances_to_world_centre);
	world->chunkmap = NULL;
	world_fill_chunk_positions(world, initchunks, arrlenu(initchunks));
	printf("hmlen is %d!\n", (i32)hmlen(world->chunkmap));
}

void world_fill_chunk_positions(World *world, ChunkPosition *positions, u32 size)
{
	SetTraceLogLevel(LOG_WARNING);
	for (u32 i = 0; i < size; i++) {
		Chunk chunk;
		init_chunk(&chunk, world, positions[i]);
		hmput(world->chunkmap, positions[i], chunk);
	}
	printf("hmlen is %d!\n", (i32)hmlen(world->chunkmap));
	SetTraceLogLevel(LOG_INFO);
}

Chunk *world_get_chunk(World *world, ChunkPosition cpos)
{
	ChunkmapKV *kv = hmgetp_null(world->chunkmap, cpos);
	if (kv == NULL) return NULL;
	return &kv->value;
}
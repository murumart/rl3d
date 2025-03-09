#include "world.h"
#include "chunks.h"

#include "lang.h"
#include "ext/stb_ds.h"

#include <stdio.h>

void init_world(World *world)
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
	fill_chunk_positions(initchunks, arrlenu(initchunks), &world->chunkmap);
	printf("hmlen is %d!\n", (i32)hmlen(world->chunkmap));
}
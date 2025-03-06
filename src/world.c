#include "world.h"

#include "lang.h"
#include "ext/stb_ds.h"

#include <stdio.h>

void init_world(World *world)
{
	const i32 WDIMS = 1;
	Vector3 *initchunks = NULL;
	for (i32 x = -WDIMS; x < WDIMS; x++)
		for (i32 y = -WDIMS; y < WDIMS; y++)
			for (i32 z = -WDIMS; z < WDIMS; z++) {
				Vector3 vec = { (float)x, (float)y, (float)z };
				arrput(initchunks, vec);
			}

	world->chunkmap = NULL;
	fill_chunk_positions(initchunks, arrlenu(initchunks), &world->chunkmap);
	printf("hmlen is %ld!\n", hmlen(world->chunkmap));
}
#include "world.h"

#include "lang.h"
#include "stb_ds.h"

#include <stdio.h>

void init_world(World *world)
{
	Vector3 *initchunks = NULL;
	for (i32 x = -5; x < 5; x++)
		for (i32 y = -5; y < 5; y++)
			for (i32 z = -5; z < 5; z++) {
				Vector3 vec = { x, y, z };
				arrput(initchunks, vec);
			}

	ChunkmapKV *chunkmap = NULL;
	fill_chunk_positions(initchunks, arrlenu(initchunks), &chunkmap);
	printf("hmlen is %ld!\n", hmlen(chunkmap));
}
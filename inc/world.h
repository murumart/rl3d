#ifndef __WORLD_H__
#define __WORLD_H__

#include "lang.h"

#include "ext/raylib.h"

// do not include chunks.h, declare opaque types here
typedef struct chunk_position ChunkPosition;
typedef struct chunkmap_kv ChunkmapKV;
typedef struct chunk Chunk;

typedef struct world {
	ChunkmapKV *chunkmap;
	Vector3 *loader_centre;
} World;

void world_init(World *world);
void world_fill_chunk_positions(World *world, ChunkPosition *positions, u32 size);

Chunk *world_get_chunk(World *world, ChunkPosition cpos);

#endif // __WORLD_H__
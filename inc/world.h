#ifndef __WORLD_H__
#define __WORLD_H__

#include "lang.h"

#include "ext/raylib.h"

#define VERTICAL_CHUNKS_LOAD 8	  // load vertically this many chunks
#define DEFAULT_LOAD_DISTANCE 24u // load horizontally this many chunks from centre

// do not include chunks.h, declare opaque types here
typedef struct chunk_position ChunkPosition;
typedef struct chunkmap_kv ChunkmapKV;
typedef struct chunk Chunk;

typedef struct state GameState;

typedef struct world {
	ChunkmapKV *chunkmap;

	ChunkPosition *load_chunk_positions;
	u32 load_chunk_positions_size;
	Vector3 *loader_centre;
	u16 view_distance;
} World;

void world_init(World *world, GameState *state);
void world_fill_chunk_positions(World *world, ChunkPosition *positions, u32 size);

Chunk *world_get_chunk(World *world, ChunkPosition cpos);

void world_update_view_distance(World *world, u16 view_distance);
void world_process_loading(GameState *state);

#endif // __WORLD_H__
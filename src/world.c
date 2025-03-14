#include "world.h"
#include "chunks.h"

#include "state.h"

#include "lang.h"
#include "ext/stb_ds.h"

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

#define VERTICAL_CHUNKS_LOAD 6 // load vertically this many chunks

static i32 compare_cpos_distances_to_world_centre(const void *a, const void *b)
{
	const ChunkPosition *ca = a;
	const ChunkPosition *cb = b;
	i32 distsa = ca->x * ca->x + ca->y * ca->y + ca->z + ca->z;
	i32 distsb = cb->x * cb->x + cb->y * cb->y + cb->z + cb->z;
	return distsa - distsb;
}

void world_init(World *world, GameState *state)
{
	world->chunkmap = NULL;
	world->loader_centre = NULL;
	world->load_chunk_positions = NULL;

	world->loader_centre = &state->camera.position;
	world_update_view_distance(world, 4);

	ChunkPosition zero = { 0 };
	world_fill_chunk_positions(world, &zero, 1);
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

static inline u32 diamond_arrsize(u16 view_distance)
{
	assert(view_distance > 0 && view_distance < 128);
	const u32 n = view_distance * 2 + 1;
	// arithmetic series sum
	const u32 a = 1;
	const u32 d = 2;
	float ar1 = ((n / 2 + 1) / 2.0) * (2 * a + ((n / 2 + 1) - 1) * d);
	float ar2 = ((n / 2) / 2.0) * (2 * a + ((n / 2) - 1) * d);
	return (u32)(ar1 + ar2);
}

// need the array size to be set properly, otherwise we're in big trouble
static inline u32 calc_visible_chunk_layer(u32 viewdist, ChunkPosition centre, ChunkPosition *loadus, u32 index)
{
	u32 ix = index;
	for (i32 zi = 0; zi < viewdist + 1; zi++) {
		i32 x = viewdist - zi - 1;
		for (i32 xi = 0; xi < zi * 2 + 1; xi++) {
			x += 1;
			loadus[ix] = (ChunkPosition){ .x = centre.x + x - viewdist,
						      .y = centre.y,
						      .z = centre.z + zi - viewdist };
			ix++;
		}
	}
	for (i32 zi = viewdist - 1; zi >= 0; zi--) {
		i32 x = viewdist - zi - 1;
		for (i32 xi = 0; xi < zi * 2 + 1; xi++) {
			x += 1;
			loadus[ix] = (ChunkPosition){ .x = centre.x + x - viewdist,
						      .y = centre.y,
						      .z = centre.z + viewdist * 2 - zi - viewdist };
			ix++;
		}
	}
	return ix;
}

// make sure the memory is allocated through world_update_view_distance
static void recalc_visible_chunks(World *world)
{
	u32 vd = world->view_distance;
	u32 index = 0;
	if (world->loader_centre == NULL) return;
	ChunkPosition centre = chunkpos_from_worldpos(*world->loader_centre);
	for (i32 y = centre.y - VERTICAL_CHUNKS_LOAD / 2; y < centre.y + VERTICAL_CHUNKS_LOAD / 2; y++) {
		ChunkPosition tc = centre;
		tc.y = y;
		index = calc_visible_chunk_layer(vd, tc, world->load_chunk_positions, index);
	}
}

void world_update_view_distance(World *world, u16 view_distance)
{
	bool differ = world->view_distance != view_distance;
	if (differ) {
		world->view_distance = view_distance;

		void *recresult = realloc(world->load_chunk_positions, sizeof(*world->load_chunk_positions)
									       * diamond_arrsize(view_distance)
									       * VERTICAL_CHUNKS_LOAD);
		assert(recresult != NULL);
		world->load_chunk_positions = recresult;
	}
	recalc_visible_chunks(world);
}

void world_process_loading(GameState *state)
{
	World *world = &state->world;

	// go over chunks that in vis range and load them
	u32 sze = diamond_arrsize(world->view_distance) * VERTICAL_CHUNKS_LOAD;
	for (u32 i = 0; i < sze; i++) {
		ChunkPosition pos2load = world->load_chunk_positions[i];
		ChunkmapKV *atpos = hmgetp_null(world->chunkmap, pos2load);
		if (atpos != NULL) continue;
		Chunk c = (Chunk){ 0 };
		init_chunk(&c, world, pos2load);
		hmput(world->chunkmap, pos2load, c);
	}

	ChunkPosition old = chunkpos_from_worldpos(state->last_loader_position);
	ChunkPosition current = chunkpos_from_worldpos(*world->loader_centre);
	if (are_chunkposes_equal(old, current)) return;

	recalc_visible_chunks(world);
}

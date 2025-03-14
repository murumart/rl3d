#include "chunks.h"
#include "chunk_meshing.h"

#include "ext/raylib.h"
#include "ext/raymath.h"
#include "ext/stb_ds.h"
#include "ext/stb_perlin.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>
#include <time.h>

u32 block_index(u32 x, u32 y, u32 z)
{
	return x + y * CHUNK_WIDTH + z * CHUNK_HEIGHT * CHUNK_WIDTH;
}

u32 block_indexp(BlockPosition bpos)
{
	return bpos.x + bpos.y * CHUNK_WIDTH + bpos.z * CHUNK_HEIGHT * CHUNK_WIDTH;
}

BlockPosition blockpos_from_chunkpos(ChunkPosition cpos)
{
	return (BlockPosition){ .x = cpos.x * CHUNK_WIDTH, .y = cpos.y * CHUNK_HEIGHT, cpos.z = cpos.z * CHUNK_WIDTH };
}

ChunkPosition chunkpos_from_blockpos(BlockPosition bpos)
{
	return (ChunkPosition){ .x = (i32)floorf(bpos.x / (float)CHUNK_WIDTH),
				.y = (i32)floorf(bpos.y / (float)CHUNK_HEIGHT),
				.z = (i32)floorf(bpos.z / (float)CHUNK_WIDTH) };
}

BlockPosition chunk_world_bpos_to_local(BlockPosition global)
{
	return (BlockPosition){ (u32)global.x % CHUNK_WIDTH, (u32)global.y % CHUNK_HEIGHT,
				(u32)global.z % CHUNK_WIDTH };
}

bool are_chunkposes_equal(ChunkPosition a, ChunkPosition b)
{
	return a.x == b.x && a.y == b.y && a.z == b.z;
}

static inline void determine_block(Chunk *chunk, BlockPosition bpos, u32 x, u32 y, u32 z)
{
	const float nscale = 0.02;
	const i32 maxy = 16;
	const float ydistr = 0.03;

	Vector3 wpos = { bpos.x + (i32)x, bpos.y + (i32)y, bpos.z + (i32)z };
	Vector3 pick = Vector3Scale(wpos, nscale);
	float density = stb_perlin_noise3(pick.x, pick.y, pick.z, 0, 0, 0);
	pick = Vector3Scale(pick, 2);
	density += stb_perlin_noise3(pick.x, pick.y, pick.z, 0, 0, 0) * 0.75;
	pick = Vector3Scale(pick, 2);
	density += stb_perlin_noise3(pick.x, pick.y, pick.z, 0, 0, 0) * 0.5;

	float ydist = wpos.y - maxy;
	//if (wpos.y > maxy) density -= ydist * ydistr;
	density -= ydist * ydistr;

	u32 i = block_index(x, y, z);
	if (density > 0) {
		chunk->block_data[i] = 1;
	}
}

void init_chunk(Chunk *chunk, World *world, ChunkPosition cpos)
{
	chunk->position = cpos;
	chunk->block_data = calloc(CHUNK_BLOCK_COUNT, sizeof(u16));
	chunk->world = world;
	BlockPosition wpos = blockpos_from_chunkpos(cpos);

	BLOCKS_ZYX_LOOP(CHUNK_WIDTH, CHUNK_HEIGHT, CHUNK_WIDTH)
	{
		determine_block(chunk, wpos, x, y, z);
	}
}

void draw_chunk(Chunk *chunk, Material material)
{
	BlockPosition cbpos = blockpos_from_chunkpos(chunk->position);
	Vector3 wpos = { cbpos.x, cbpos.y, cbpos.z };
	/* BLOCKS_ZYX_LOOP(CHUNK_WIDTH, CHUNK_HEIGHT, CHUNK_WIDTH)
	{
		u16 block = chunk->block_data[block_index(x, y, z)];
		if (block == 0) continue;
		Vector3 drawposition = Vector3Add(wpos, (Vector3){ x, y, z });
		DrawPoint3D(drawposition, GREEN);
	} */

	Vector3 chunkcentre = Vector3Multiply(Vector3One(), Vector3Scale(CHUNK_DIM, 0.5));
	DrawCubeWiresV(Vector3Add(wpos, chunkcentre), Vector3Multiply(Vector3One(), CHUNK_DIM), BLUE);

	DrawMesh(chunk->mesh, material, MatrixTranslate(wpos.x, wpos.y, wpos.z));
}

void process_chunk(Chunk *chunk, float delta)
{
}

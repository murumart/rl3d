#include "chunks.h"
#include "chunk_meshing.h"

#include "ext/raylib.h"
#include "ext/raymath.h"
#include "ext/stb_ds.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>

u32 block_index(u32 x, u32 y, u32 z)
{
	return x + y * CHUNK_WIDTH + z * CHUNK_HEIGHT * CHUNK_WIDTH;
}

u32 block_indexp(BlockPosition bpos)
{
	return bpos.x + bpos.y * CHUNK_WIDTH + bpos.z * CHUNK_HEIGHT * CHUNK_WIDTH;
}

BlockPosition chunkpos_to_blockpos(ChunkPosition cpos)
{
	return (BlockPosition){ .x = cpos.x * CHUNK_WIDTH, .y = cpos.y * CHUNK_HEIGHT, cpos.z = cpos.z * CHUNK_WIDTH };
}

static inline void determine_block(Chunk *chunk, BlockPosition bpos, u32 x, u32 y, u32 z)
{
	i32 height = bpos.y + y;
	height = 32 + height;

	u32 i = block_index(x, y, z);
	if (GetRandomValue(0, height < 0 ? 0 : height * height) == 0) {
		chunk->block_data[i] = 1;
	}
}

void init_chunk(Chunk *chunk, ChunkPosition cpos)
{
	chunk->position = cpos;
	chunk->block_data = calloc(CHUNK_BLOCK_COUNT, sizeof(u16));
	BlockPosition wpos = chunkpos_to_blockpos(cpos);

	BLOCKS_ZYX_LOOP(CHUNK_WIDTH, CHUNK_HEIGHT, CHUNK_WIDTH)
	{
		determine_block(chunk, wpos, x, y, z);
	}
	mesh_chunk(chunk);
}

void fill_chunk_positions(ChunkPosition *positions, u32 size, ChunkmapKV **chunkmap)
{
	for (u32 i = 0; i < size; i++) {
		Chunk chunk;
		init_chunk(&chunk, positions[i]);
		hmput(*chunkmap, positions[i], chunk);
	}
	printf("hmlen is %d!\n", (i32)hmlen(*chunkmap));
}

void draw_chunk(Chunk *chunk, Material material)
{
	BlockPosition cbpos = chunkpos_to_blockpos(chunk->position);
	Vector3 wpos = { cbpos.x, cbpos.y, cbpos.z };
	/* BLOCKS_ZYX_LOOP(CHUNK_WIDTH, CHUNK_HEIGHT, CHUNK_WIDTH)
	{
		u16 block = chunk->block_data[block_index(x, y, z)];
		if (block == 0) continue;
		Vector3 drawposition = Vector3Add(wpos, (Vector3){ x, y, z });
		DrawCubeV(drawposition, Vector3One(), GREEN);
		DrawCubeWiresV(drawposition, Vector3One(), DARKGREEN);
	} */

	DrawCubeWiresV(Vector3Add(wpos, Vector3Multiply(Vector3One(), Vector3Scale(CHUNK_DIM, 0.5))),
		       Vector3Multiply(Vector3One(), CHUNK_DIM), BLUE);

	DrawMesh(chunk->mesh, material, MatrixTranslate(wpos.x, wpos.y, wpos.z));
}

void process_chunk(Chunk *chunk, float delta)
{
}

#include "chunks.h"

#include "raylib.h"
#include "raymath.h"
#include "stb_ds.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

u32 block_index(u32 x, u32 y, u32 z)
{
	return x + y * CHUNK_WIDTH + z * CHUNK_HEIGHT * CHUNK_WIDTH;
}

Vector3 chunk_world_position(Vector3 chunkpos)
{
	return Vector3Multiply(chunkpos, CHUNK_DIM);
}

static inline void determine_block(Chunk *chunk, Vector3 wpos, u32 x, u32 y, u32 z)
{
	i32 density = ((i32)y + wpos.y * wpos.y * wpos.y);
	density = density < 0 ? 0 : density;

	u32 i = block_index(x, y, z);
	if (GetRandomValue(0, density) == 0) {
		chunk->block_data[i] = 1;
	}
}

void init_chunk(Chunk *chunk, Vector3 cpos)
{
	chunk->position = cpos;
	chunk->block_data = calloc(CHUNK_BLOCK_COUNT, sizeof(u16));
	Vector3 wpos = Vector3Multiply(cpos, CHUNK_DIM);

	for (u32 z = 0; z < CHUNK_WIDTH; z++) {
		for (u32 y = 0; y < CHUNK_HEIGHT; y++) {
			for (u32 x = 0; x < CHUNK_WIDTH; x++) {
				determine_block(chunk, wpos, x, y, z);
			}
		}
	}
}

void fill_chunk_positions(Vector3 *positions, u32 size, ChunkmapKV **chunkmap)
{
	for (u32 i = 0; i < size; i++) {
		Chunk chunk;
		init_chunk(&chunk, positions[i]);
		hmput(*chunkmap, positions[i], chunk);
	}
	printf("hmlen is %ld!\n", hmlen(*chunkmap));
}

void draw_chunk(Chunk *chunk)
{
	Vector3 wpos = chunk_world_position(chunk->position);
	for (u32 z = 0; z < CHUNK_WIDTH; z++) {
		for (u32 y = 0; y < CHUNK_HEIGHT; y++) {
			for (u32 x = 0; x < CHUNK_WIDTH; x++) {
				u16 block = chunk->block_data[block_index(x, y, z)];
				if (block == 0) continue;
				Vector3 drawposition = Vector3Add(wpos, (Vector3){ x, y, z });
				DrawCubeV(drawposition, Vector3One(), GREEN);
				DrawCubeWiresV(drawposition, Vector3One(), DARKGREEN);
			}
		}
	}
	DrawCubeWiresV(Vector3Add(wpos, Vector3Multiply(Vector3One(), Vector3Scale(CHUNK_DIM, 0.5))),
		       Vector3Multiply(Vector3One(), CHUNK_DIM), BLUE);
}

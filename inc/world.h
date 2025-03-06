#ifndef __WORLD_H__
#define __WORLD_H__

#include "lang.h"

#include "chunks.h"

#include "raylib.h"

typedef struct world {
	ChunkmapKV *chunkmap;
} World;

void init_world(World *world);

#endif // __WORLD_H__
#ifndef __CHUNK_MESHING_H__
#define __CHUNK_MESHING_H__

#include "lang.h"

#include "chunks.h"

bool chunk_mesh_mesh(Chunk *chunk, World *world);
bool chunk_mesh_destroy(Chunk *chunk);

#endif // __CHUNK_MESHING_H__
#pragma once

#include "vec3.h"
#include <stdint.h>

typedef enum BLOCK_ID { NONE, AIR, GRASS } BLOCK_ID;

typedef struct VoxelNode {
  uint16_t block_id; // 0 = has octants
  struct VoxelNode *octants[8];
} VoxelNode;

typedef struct Chunk {
  VoxelNode root;
  unsigned int vertex_buffer;
  unsigned int voxel_buffer;
  unsigned int mesh_size;
} Chunk;

void chunk_init(Chunk *chunk);

BLOCK_ID chunk_get_block_id(const Chunk *chunk, const Vec3 block);

void chunk_build_mesh(Chunk *chunk);

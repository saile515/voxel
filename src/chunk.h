#pragma once

#include "vec3.h"
#include "vector.h"
#include <stdint.h>
#include <threads.h>

typedef enum BLOCK_ID { NONE, AIR, GRASS } BLOCK_ID;

typedef struct Mesh {
  Vector_float vertices;
  Vector_float normals;
} Mesh;

typedef struct VoxelNode {
  uint16_t block_id; // 0 = has octants
  struct VoxelNode *octants[8];
} VoxelNode;

typedef struct Chunk {
  VoxelNode root;
  unsigned int vertex_buffer;
  unsigned int normal_buffer;
  unsigned int mesh_size;
  Vec3 position;
  mtx_t mutex;
} Chunk;

void chunk_init(Chunk *chunk, const Vec3 position);

BLOCK_ID chunk_get_block_id(const Chunk *chunk, const Vec3 block);

Mesh chunk_build_mesh(Chunk *chunk);

void voxel_node_free(VoxelNode *voxel_node);

void chunk_free(Chunk *chunk);

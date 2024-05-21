#pragma once

#include "block_type.h"
#include "vec3.h"
#include "vector.h"
#include <stdint.h>
#include <threads.h>

typedef struct World World;

typedef struct Mesh {
  Vector_float vertices;
  Vector_float normals;
} Mesh;

typedef struct VoxelNode {
  bool has_octants;
  BlockType block_type;
  struct VoxelNode *octants[8];
} VoxelNode;

typedef struct Chunk {
  VoxelNode root;
  unsigned int vertex_buffer;
  unsigned int normal_buffer;
  unsigned int mesh_size;
  Vec3i position;
  mtx_t mutex;
} Chunk;

void chunk_init(Chunk *chunk, const Vec3i position);

BlockType chunk_get_block_type(const Chunk *chunk, const Vec3i block);

uint64_t *chunk_build_block_mask(Chunk *chunk, World *world, unsigned int axis);

bool chunk_block_is_solid(const Chunk *chunk, const Vec3i position);

Mesh chunk_build_mesh(Chunk *chunk, World *world);

void voxel_node_free(VoxelNode *voxel_node);

void chunk_free(Chunk *chunk);

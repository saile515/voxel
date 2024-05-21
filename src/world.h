#pragma once

#include "camera.h"
#include "chunk.h"
#include <threads.h>

#define render_distance 16

typedef struct VoxelShader {
  unsigned int program_id;
  unsigned int vertex_position_attribute;
  unsigned int vertex_normal_attribute;
  unsigned int projection_matrix_uniform;
  unsigned int view_matrix_uniform;
  unsigned int chunk_position_uniform;
} VoxelShader;

typedef struct ChunkThreadData {
  Chunk **chunks;
  Mesh *out;
  unsigned int size;
  bool world_thread_busy;
  World *world;
} ChunkThreadData;

typedef struct World {
  Chunk *chunks[render_distance][render_distance][render_distance];
  VoxelShader shader;
  thrd_t world_thread;
  ChunkThreadData chunk_thread_data;
} World;

void world_init(World *world);

Chunk *world_get_chunk(const World *world, const Vec3i position);

void world_load(World *world, Camera *camera);

void world_render(World *world, Camera *camera);

void world_free(World *world);

#pragma once

#include "camera.h"
#include "chunk.h"

#define render_distance 8

typedef struct VoxelShader {
  unsigned int program_id;
  unsigned int vertex_position_attribute;
  unsigned int vertex_normal_attribute;
  unsigned int projection_matrix_uniform;
  unsigned int view_matrix_uniform;
  unsigned int chunk_position_uniform;
} VoxelShader;

typedef struct World {
  Chunk *chunks[render_distance][render_distance][render_distance];
  VoxelShader shader;
} World;

void world_init(World *world);

void world_load(World *world, Camera *camera);

void world_render(World *world, Camera *camera);

void world_free(World *world);

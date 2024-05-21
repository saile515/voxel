#include "chunk.h"

#include "block_type.h"
#include "tracy/TracyC.h"
#include "vec3.h"
#include "vector.h"
#include "world.h"
#include <GL/glew.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

static void voxel_node_init(VoxelNode *voxel_node, int depth) {
  voxel_node->has_octants = true;

  for (uint8_t i = 0; i < 8; i++) {
    voxel_node->octants[i] = calloc(1, sizeof(VoxelNode));
  }

  voxel_node->octants[0]->block_type = &GRASS;
  voxel_node->octants[1]->block_type = &AIR;
  voxel_node->octants[2]->block_type = &AIR;
  voxel_node->octants[3]->block_type = &GRASS;
  voxel_node->octants[4]->block_type = &AIR;
  voxel_node->octants[5]->block_type = &GRASS;
  voxel_node->octants[6]->block_type = &GRASS;
  voxel_node->octants[7]->block_type = &AIR;
}

void chunk_init(Chunk *chunk, const Vec3i position) {
  TracyCZone(chunk_init, true);

  mtx_init(&chunk->mutex, mtx_plain);

  vec3i_copy(chunk->position, position);

  voxel_node_init(&chunk->root, 0);

  glGenBuffers(1, &chunk->vertex_buffer);
  glGenBuffers(1, &chunk->normal_buffer);

  chunk->mesh_size = 0;

  TracyCZoneEnd(chunk_init);
}

BlockType chunk_get_block_type(const Chunk *chunk, const Vec3i block) {
  const VoxelNode *octant = &chunk->root;

  int depth_factor = 16;

  uint8_t local_x = block[0];
  uint8_t local_y = block[1];
  uint8_t local_z = block[2];

  while (octant->has_octants) {
    int octant_x = local_x / depth_factor < 1;
    int octant_y = local_y / depth_factor < 1;
    int octant_z = local_z / depth_factor < 1;

    local_x %= depth_factor;
    local_y %= depth_factor;
    local_z %= depth_factor;

    octant = octant->octants[octant_x + octant_y * 2 + octant_z * 4];

    depth_factor /= 2;
  }

  return octant->block_type;
}

static void make_vertex(Vector_float *vertices, Vector_float *normals,
                        Vec3i coordinates, unsigned int normal) {
  vector_insert_float(vertices, coordinates[0] + coordinates[1] * 33 +
                                    coordinates[2] * 33 * 33);
  vector_insert_float(normals, normal);
}

static void make_face(Vector_float *vertices, Vector_float *normals,
                      Vec3i coordinates, int axis, int negative) {
  bool axis_x = axis == 0;
  bool axis_y = axis == 1;
  bool axis_z = axis == 2;

  Vec3i v1 = {coordinates[0], coordinates[1], coordinates[2]};
  Vec3i v2 = {coordinates[0] + !axis_x, coordinates[1] + !axis_y,
              coordinates[2] + !axis_z};
  Vec3i v3 = {coordinates[0] + axis_z, coordinates[1] + axis_x,
              coordinates[2] + axis_y};
  Vec3i v4 = {coordinates[0] + axis_y, coordinates[1] + axis_z,
              coordinates[2] + axis_x};

  float normal = !negative * 3 + axis;

  if (negative == 0) {
    make_vertex(vertices, normals, v3, normal);
    make_vertex(vertices, normals, v2, normal);
    make_vertex(vertices, normals, v1, normal);
    make_vertex(vertices, normals, v2, normal);
    make_vertex(vertices, normals, v4, normal);
    make_vertex(vertices, normals, v1, normal);
  } else {
    make_vertex(vertices, normals, v1, normal);
    make_vertex(vertices, normals, v2, normal);
    make_vertex(vertices, normals, v3, normal);
    make_vertex(vertices, normals, v1, normal);
    make_vertex(vertices, normals, v4, normal);
    make_vertex(vertices, normals, v2, normal);
  }
}

static void build_block_mask_step(uint64_t *block_mask, const VoxelNode *node,
                                  unsigned int depth_factor, const Vec3i offset,
                                  unsigned int axis) {
  if (node->has_octants) {
    for (unsigned char x = 0; x < 2; x++) {
      for (unsigned char y = 0; y < 2; y++) {
        for (unsigned char z = 0; z < 2; z++) {
          build_block_mask_step(block_mask, node->octants[x + y * 2 + z * 4],
                                depth_factor / 2,
                                (Vec3i){offset[0] + x * depth_factor / 2,
                                        offset[1] + y * depth_factor / 2,
                                        offset[2] + z * depth_factor / 2},
                                axis);
        }
      }
    }

    return;
  }

  if (node->block_type->is_solid == false) {
    return;
  }

  uint64_t mask = 1;
  for (unsigned int i = 0; i < depth_factor - 1; i++) {
    mask = mask << 1;
    mask++;
  }

  for (int a = 0; a < depth_factor; a++) {
    for (int b = 0; b < depth_factor; b++) {
      switch (axis) {
      case 0:
        block_mask[offset[1] + a + (offset[2] + b) * 32] |= mask << offset[0];
      case 1:
        block_mask[offset[0] + a + (offset[2] + b) * 32] |= mask << offset[1];
      case 2:
        block_mask[offset[0] + a + (offset[1] + b) * 32] |= mask << offset[2];
      }
    }
  }
}

uint64_t *chunk_build_block_mask(Chunk *chunk, World *world,
                                 unsigned int axis) {
  TracyCZone(chunk_build_block_mask, true);

  uint64_t *block_mask = calloc(1024, sizeof(uint64_t));
  build_block_mask_step(block_mask, &chunk->root, 32, (Vec3i){0, 0, 0}, axis);

  bool axis_x = axis == 0;
  bool axis_y = axis == 1;
  bool axis_z = axis == 2;

  Chunk *previous_chunk = world_get_chunk(
      world, (Vec3i){chunk->position[0] - axis_x, chunk->position[1] - axis_y,
                     chunk->position[2] - axis_z});
  Chunk *next_chunk = world_get_chunk(
      world, (Vec3i){chunk->position[0] + axis_x, chunk->position[1] + axis_y,
                     chunk->position[2] + axis_z});

  for (unsigned int a = 0; a < 32; a++) {
    for (unsigned int b = 0; b < 32; b++) {
      Vec3i previous_block_position = {axis_x ? 31 : a,
                                       axis_y   ? 31
                                       : axis_x ? a
                                                : b,
                                       axis_z ? 31 : b};
      uint64_t previous_block =
          chunk_get_block_type(previous_chunk, previous_block_position)
              ->is_solid;

      block_mask[a + b * 32] |= previous_block << 32;

      Vec3i next_block_position = {axis_x ? 0 : a,
                                   axis_y   ? 0
                                   : axis_x ? a
                                            : b,
                                   axis_z ? 0 : b};
      uint64_t next_block =
          chunk_get_block_type(next_chunk, next_block_position)->is_solid;

      block_mask[a + b * 32] <<= 1;

      block_mask[a + b * 32] |= next_block;
    }
  }

  TracyCZoneEnd(chunk_build_block_mask);
  return block_mask;
}

static void faces_from_block_mask(Vector_float *vertices, Vector_float *normals,
                                  uint64_t *block_mask, unsigned int axis,
                                  bool negative) {
  TracyCZone(faces_from_block_mask, true);
  for (unsigned int a = 0; a < 32; a++) {
    for (unsigned int b = 0; b < 32; b++) {
      uint64_t face_mask = block_mask[a + b * 32];
      face_mask &= ~(negative ? face_mask << 1 : face_mask >> 1);

      for (unsigned int c = 0; c < 32; c++) {
        face_mask >>= 1;
        if (face_mask & 1) {
          Vec3i coordinates = {axis == 0 ? c : a,
                               axis == 1   ? c
                               : axis == 0 ? a
                                           : b,
                               axis == 2 ? c : b};

          if (!negative) {
            vec3i_add(coordinates, coordinates,
                      (Vec3i){axis == 0, axis == 1, axis == 2});
          }

          make_face(vertices, normals, coordinates, axis, negative);
        }
      }
    }
  }
  TracyCZoneEnd(faces_from_block_mask);
}

Mesh chunk_build_mesh(Chunk *chunk, World *world) {
  TracyCZone(chunk_build_mesh, true);

  if (!chunk->root.has_octants && chunk->root.block_type == &AIR) {
    TracyCZoneEnd(chunk_build_mesh);
    return (Mesh){};
  }

  Vector_float vertices = {0};
  Vector_float normals = {0};

  vector_init_float(&vertices, 64);
  vector_init_float(&normals, 64);

  uint64_t *block_mask_x = chunk_build_block_mask(chunk, world, 0);
  uint64_t *block_mask_y = chunk_build_block_mask(chunk, world, 1);
  uint64_t *block_mask_z = chunk_build_block_mask(chunk, world, 2);

  faces_from_block_mask(&vertices, &normals, block_mask_x, 0, 0);
  faces_from_block_mask(&vertices, &normals, block_mask_x, 0, 1);
  faces_from_block_mask(&vertices, &normals, block_mask_x, 1, 0);
  faces_from_block_mask(&vertices, &normals, block_mask_x, 1, 1);
  faces_from_block_mask(&vertices, &normals, block_mask_x, 2, 0);
  faces_from_block_mask(&vertices, &normals, block_mask_x, 2, 1);

  free(block_mask_x);
  free(block_mask_y);
  free(block_mask_z);

  TracyCZoneEnd(chunk_build_mesh);

  return (Mesh){vertices, normals};
}

void voxel_node_free(VoxelNode *voxel_node) {
  if (voxel_node == NULL) {
    return;
  }

  if (voxel_node->has_octants) {
    for (unsigned int i = 0; i < 8; i++) {
      voxel_node_free(voxel_node->octants[i]);
      free(voxel_node->octants[i]);
    }
  }
}

void chunk_free(Chunk *chunk) {
  voxel_node_free(&chunk->root);
  mtx_destroy(&chunk->mutex);
}

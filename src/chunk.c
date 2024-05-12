#include "chunk.h"

#include "vec3.h"
#include "vector.h"
#include <GL/glew.h>
#include <math.h>
#include <stdint.h>
#include <stdlib.h>

void chunk_init(Chunk *chunk) {
  for (uint8_t i = 0; i < 8; i++) {
    chunk->root.octants[i] = calloc(1, sizeof(VoxelNode));
    chunk->root.octants[i]->block_id = AIR;
  }

  chunk->root.octants[0]->block_id = GRASS;
  chunk->root.octants[3]->block_id = GRASS;
  chunk->root.octants[5]->block_id = GRASS;
  chunk->root.octants[6]->block_id = GRASS;

  glGenBuffers(1, &chunk->vertex_buffer);
  glGenBuffers(1, &chunk->voxel_buffer);

  chunk->mesh_size = 0;
}

BLOCK_ID chunk_get_block_id(const Chunk *chunk, const Vec3 block) {
  const VoxelNode *octant = &chunk->root;

  uint16_t block_id = chunk->root.block_id;
  int depth = 1;

  uint8_t local_x = block[0];
  uint8_t local_y = block[1];
  uint8_t local_z = block[2];

  while (block_id == NONE) {
    int depth_factor = (32 / (int)pow(2, depth));

    int octant_x = local_x / depth_factor < 1 ? 0 : 1;
    int octant_y = local_y / depth_factor < 1 ? 0 : 1;
    int octant_z = local_z / depth_factor < 1 ? 0 : 1;

    local_x %= depth_factor;
    local_y %= depth_factor;
    local_z %= depth_factor;

    octant = octant->octants[octant_x + octant_y * 2 + octant_z * 4];

    block_id = octant->block_id;
  }

  return block_id;
}

static void make_face(Vector_float *vertices, Vec3 coordinates, int axis,
                      int normal) {
  Vec3 v1 = {coordinates[0], coordinates[1], coordinates[2]};
  Vec3 v2 = {coordinates[0] + (axis == 0 ? 0 : 1),
             coordinates[1] + (axis == 1 ? 0 : 1),
             coordinates[2] + (axis == 2 ? 0 : 1)};
  Vec3 v3 = {coordinates[0] + (axis == 2 ? 1 : 0),
             coordinates[1] + (axis == 0 ? 1 : 0),
             coordinates[2] + (axis == 1 ? 1 : 0)};
  Vec3 v4 = {coordinates[0] + (axis == 1 ? 1 : 0),
             coordinates[1] + (axis == 2 ? 1 : 0),
             coordinates[2] + (axis == 0 ? 1 : 0)};

  if (normal == 0) {
    vector_insert_float(vertices, v1[0] + v1[1] * 33 + v1[2] * 33 * 33);
    vector_insert_float(vertices, v2[0] + v2[1] * 33 + v2[2] * 33 * 33);
    vector_insert_float(vertices, v3[0] + v3[1] * 33 + v3[2] * 33 * 33);
    vector_insert_float(vertices, v1[0] + v1[1] * 33 + v1[2] * 33 * 33);
    vector_insert_float(vertices, v4[0] + v4[1] * 33 + v4[2] * 33 * 33);
    vector_insert_float(vertices, v2[0] + v2[1] * 33 + v2[2] * 33 * 33);
  } else {
    vector_insert_float(vertices, v3[0] + v3[1] * 33 + v3[2] * 33 * 33);
    vector_insert_float(vertices, v2[0] + v2[1] * 33 + v2[2] * 33 * 33);
    vector_insert_float(vertices, v1[0] + v1[1] * 33 + v1[2] * 33 * 33);
    vector_insert_float(vertices, v2[0] + v2[1] * 33 + v2[2] * 33 * 33);
    vector_insert_float(vertices, v4[0] + v4[1] * 33 + v4[2] * 33 * 33);
    vector_insert_float(vertices, v1[0] + v1[1] * 33 + v1[2] * 33 * 33);
  }
}

void chunk_build_mesh(Chunk *chunk) {
  Vector_float vertices = {0};

  vector_init_float(&vertices, 8);

  for (unsigned int x = 0; x < 32; x++) {
    for (unsigned int y = 0; y < 32; y++) {
      for (unsigned int z = 0; z < 32; z++) {
        if (chunk_get_block_id(chunk, (Vec3){x, y, z}) == AIR) {
          continue;
        }

        if (x == 0 || chunk_get_block_id(chunk, (Vec3){x - 1, y, z}) == AIR) {
          make_face(&vertices, (Vec3){x, y, z}, 0, 0);
        }

        if (y == 0 || chunk_get_block_id(chunk, (Vec3){x, y - 1, z}) == AIR) {
          make_face(&vertices, (Vec3){x, y, z}, 1, 0);
        }

        if (z == 0 || chunk_get_block_id(chunk, (Vec3){x, y, z - 1}) == AIR) {
          make_face(&vertices, (Vec3){x, y, z}, 2, 0);
        }

        if (x == 31 || chunk_get_block_id(chunk, (Vec3){x + 1, y, z}) == AIR) {
          make_face(&vertices, (Vec3){x + 1, y, z}, 0, 1);
        }

        if (y == 31 || chunk_get_block_id(chunk, (Vec3){x, y + 1, z}) == AIR) {
          make_face(&vertices, (Vec3){x, y + 1, z}, 1, 1);
        }

        if (z == 31 || chunk_get_block_id(chunk, (Vec3){x, y, z + 1}) == AIR) {
          make_face(&vertices, (Vec3){x, y, z + 1}, 2, 1);
        }
      }
    }
  }

  chunk->mesh_size = vertices.size;

  vector_free_float(&vertices);

  glBindBuffer(GL_ARRAY_BUFFER, chunk->vertex_buffer);
  glBufferData(GL_ARRAY_BUFFER, vertices.size * sizeof(float), vertices.data,
               GL_STATIC_DRAW);
}

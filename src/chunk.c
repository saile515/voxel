#include "chunk.h"

#include "vec3.h"
#include "vector.h"
#include <GL/glew.h>
#include <math.h>
#include <stdint.h>
#include <stdlib.h>

static void voxel_node_init(VoxelNode *voxel_node, int depth) {
  int depth_factor = pow(2, depth);

  if (depth_factor == 32) {
    voxel_node->block_id = AIR;
    return;
  }

  for (uint8_t i = 0; i < 8; i++) {
    voxel_node->octants[i] = calloc(1, sizeof(VoxelNode));
  }

  if (depth % 2 == 0) {
    voxel_node->octants[0]->block_id = AIR;
    voxel_node->octants[1]->block_id = GRASS;
    voxel_node_init(voxel_node->octants[2], depth + 1);
    voxel_node->octants[3]->block_id = AIR;
    voxel_node->octants[4]->block_id = AIR;
    voxel_node_init(voxel_node->octants[5], depth + 1);
    voxel_node->octants[6]->block_id = GRASS;
    voxel_node->octants[7]->block_id = AIR;
  } else {
    voxel_node_init(voxel_node->octants[0], depth + 1);
    voxel_node->octants[1]->block_id = GRASS;
    voxel_node->octants[2]->block_id = AIR;
    voxel_node->octants[3]->block_id = AIR;
    voxel_node->octants[4]->block_id = AIR;
    voxel_node->octants[5]->block_id = AIR;
    voxel_node->octants[6]->block_id = GRASS;
    voxel_node_init(voxel_node->octants[7], depth + 1);
  }
}

void chunk_init(Chunk *chunk, const Vec3 position) {
  vec3_copy(chunk->position, position);
  voxel_node_init(&chunk->root, 0);

  glGenBuffers(1, &chunk->vertex_buffer);
  glGenBuffers(1, &chunk->normal_buffer);

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

    depth++;
  }

  return block_id;
}

static void make_vertex(Vector_float *vertices, Vector_float *normals,
                        Vec3 coordinates, float normal) {
  vector_insert_float(vertices, coordinates[0] + coordinates[1] * 33 +
                                    coordinates[2] * 33 * 33);
  vector_insert_float(normals, normal);
}

static void make_face(Vector_float *vertices, Vector_float *normals,
                      Vec3 coordinates, int axis, int normal_sign) {
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

  float normal = ((normal_sign * 2) - 1) * axis;

  if (normal_sign == 0) {
    make_vertex(vertices, normals, v1, normal);
    make_vertex(vertices, normals, v2, normal);
    make_vertex(vertices, normals, v3, normal);
    make_vertex(vertices, normals, v1, normal);
    make_vertex(vertices, normals, v4, normal);
    make_vertex(vertices, normals, v2, normal);
  } else {
    make_vertex(vertices, normals, v3, normal);
    make_vertex(vertices, normals, v2, normal);
    make_vertex(vertices, normals, v1, normal);
    make_vertex(vertices, normals, v2, normal);
    make_vertex(vertices, normals, v4, normal);
    make_vertex(vertices, normals, v1, normal);
  }
}

void chunk_build_mesh(Chunk *chunk) {
  Vector_float vertices = {0};
  Vector_float normals = {0};

  vector_init_float(&vertices, 64);
  vector_init_float(&normals, 64);

  for (unsigned int x = 0; x < 32; x++) {
    for (unsigned int y = 0; y < 32; y++) {
      for (unsigned int z = 0; z < 32; z++) {
        if (chunk_get_block_id(chunk, (Vec3){x, y, z}) == AIR) {
          continue;
        }

        if (x == 0 || chunk_get_block_id(chunk, (Vec3){x - 1, y, z}) == AIR) {
          make_face(&vertices, &normals, (Vec3){x, y, z}, 0, 0);
        }

        if (y == 0 || chunk_get_block_id(chunk, (Vec3){x, y - 1, z}) == AIR) {
          make_face(&vertices, &normals, (Vec3){x, y, z}, 1, 0);
        }

        if (z == 0 || chunk_get_block_id(chunk, (Vec3){x, y, z - 1}) == AIR) {
          make_face(&vertices, &normals, (Vec3){x, y, z}, 2, 0);
        }

        if (x == 31 || chunk_get_block_id(chunk, (Vec3){x + 1, y, z}) == AIR) {
          make_face(&vertices, &normals, (Vec3){x + 1, y, z}, 0, 1);
        }

        if (y == 31 || chunk_get_block_id(chunk, (Vec3){x, y + 1, z}) == AIR) {
          make_face(&vertices, &normals, (Vec3){x, y + 1, z}, 1, 1);
        }

        if (z == 31 || chunk_get_block_id(chunk, (Vec3){x, y, z + 1}) == AIR) {
          make_face(&vertices, &normals, (Vec3){x, y, z + 1}, 2, 1);
        }
      }
    }
  }

  chunk->mesh_size = vertices.size;

  glBindBuffer(GL_ARRAY_BUFFER, chunk->vertex_buffer);
  glBufferData(GL_ARRAY_BUFFER, vertices.size * sizeof(float), vertices.data,
               GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, chunk->normal_buffer);
  glBufferData(GL_ARRAY_BUFFER, normals.size * sizeof(float), normals.data,
               GL_STATIC_DRAW);

  vector_free_float(&vertices);
  vector_free_float(&normals);
}

void voxel_node_free(VoxelNode *voxel_node) {
  if (voxel_node->block_id == NONE) {
    for (unsigned int i = 0; i < 8; i++) {
      voxel_node_free(voxel_node->octants[i]);
      free(voxel_node->octants[i]);
    }
  }
}

void chunk_free(Chunk *chunk) { voxel_node_free(&chunk->root); }

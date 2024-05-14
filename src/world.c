#include "world.h"

#include "camera.h"
#include "chunk.h"
#include "load_shader.h"
#include "math_util.h"
#include "tracy/TracyC.h"
#include <GL/glew.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <threads.h>

int build_chunk_mesh(void *data) {
  TracyCSetThreadName("build_chunk_mesh");
  ChunkThreadData *chunk_thread_data = data;
  chunk_thread_data->world_thread_busy = true;

  for (int i = 0; i < chunk_thread_data->size; i++) {
    Chunk *chunk = chunk_thread_data->chunks[i];
    mtx_lock(&chunk->mutex);
    Mesh *out = &chunk_thread_data->out[i];
    *out = chunk_build_mesh(chunk);
    mtx_unlock(&chunk->mutex);
  }

  chunk_thread_data->world_thread_busy = false;

  return 0;
}

void world_init(World *world) {
  world->chunk_thread_data.chunks =
      malloc(sizeof(Chunk *) * (int)(pow(render_distance, 3)));
  world->chunk_thread_data.out =
      malloc(sizeof(Mesh) * (int)(pow(render_distance, 3)));
  world->chunk_thread_data.size = 0;
  world->chunk_thread_data.world_thread_busy = false;

  world->shader.program_id = load_shader("assets/shaders/vertex_shader.glsl",
                                         "assets/shaders/fragment_shader.glsl");

  world->shader.vertex_position_attribute =
      glGetAttribLocation(world->shader.program_id, "vertex_position");
  world->shader.vertex_normal_attribute =
      glGetAttribLocation(world->shader.program_id, "vertex_normal");
  world->shader.projection_matrix_uniform =
      glGetUniformLocation(world->shader.program_id, "projection_matrix");
  world->shader.view_matrix_uniform =
      glGetUniformLocation(world->shader.program_id, "view_matrix");
  world->shader.chunk_position_uniform =
      glGetUniformLocation(world->shader.program_id, "chunk_position");

  for (int x = 0; x < render_distance; x++) {
    for (int y = 0; y < render_distance; y++) {
      for (int z = 0; z < render_distance; z++) {
        Vec3 chunk_position = {x - (float)render_distance / 2,
                               y - (float)render_distance / 2,
                               z - (float)render_distance / 2};

        int index_x = mod((int)chunk_position[0], render_distance);
        int index_y = mod((int)chunk_position[1], render_distance);
        int index_z = mod((int)chunk_position[2], render_distance);

        world->chunks[index_x][index_y][index_z] = calloc(1, sizeof(Chunk));

        Chunk *chunk = world->chunks[index_x][index_y][index_z];
        chunk_init(chunk, chunk_position);
        world->chunk_thread_data.chunks[world->chunk_thread_data.size++] =
            chunk;
      }
    }
  }

  thrd_create(&world->world_thread, build_chunk_mesh,
              &world->chunk_thread_data);
}

void world_load(World *world, Camera *camera) {
  if (world->chunk_thread_data.world_thread_busy) {
    return;
  }

  for (int i = 0; i < world->chunk_thread_data.size; i++) {
    Chunk *chunk = world->chunk_thread_data.chunks[i];
    Mesh *mesh = &world->chunk_thread_data.out[i];
    glBindBuffer(GL_ARRAY_BUFFER, chunk->vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, mesh->vertices.size * sizeof(float),
                 mesh->vertices.data, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, chunk->normal_buffer);
    glBufferData(GL_ARRAY_BUFFER, mesh->normals.size * sizeof(float),
                 mesh->normals.data, GL_STATIC_DRAW);

    chunk->mesh_size = mesh->vertices.size;

    vector_free_float(&mesh->vertices);
    vector_free_float(&mesh->normals);
  }

  world->chunk_thread_data.size = 0;

  TracyCZone(world_load, true);
  Vec3 camera_position;
  vec3_copy(camera_position, camera->transform.position);
  camera_position[0] = floor((camera_position[0]) / 32);
  camera_position[1] = floor((camera_position[1]) / 32);
  camera_position[2] = floor((camera_position[2]) / 32);

  for (int x = 0; x < render_distance; x++) {
    for (int y = 0; y < render_distance; y++) {
      for (int z = 0; z < render_distance; z++) {
        if (x != 0 && y != 0 && z != 0 && x != render_distance - 1 &&
            y != render_distance - 1 && z != render_distance - 1) {
          continue;
        }

        Vec3 chunk_position;
        vec3_add(chunk_position, camera_position,
                 (Vec3){x - (float)render_distance / 2,
                        y - (float)render_distance / 2,
                        z - (float)render_distance / 2});

        int index_x = mod((int)chunk_position[0], render_distance);
        int index_y = mod((int)chunk_position[1], render_distance);
        int index_z = mod((int)chunk_position[2], render_distance);

        Chunk *chunk = world->chunks[index_x][index_y][index_z];

        if (!vec3_compare(chunk->position, chunk_position)) {
          chunk_free(chunk);
          memset(chunk, 0, sizeof(Chunk));
          chunk_init(chunk, chunk_position);
          world->chunk_thread_data.chunks[world->chunk_thread_data.size++] =
              chunk;
        }
      }
    }
  }

  if (world->chunk_thread_data.size != 0) {
    thrd_create(&world->world_thread, build_chunk_mesh,
                &world->chunk_thread_data);
  }

  TracyCZoneEnd(world_load);
}

void world_render(World *world, Camera *camera) {
  glUseProgram(world->shader.program_id);

  glUniformMatrix4fv(world->shader.projection_matrix_uniform, 1, GL_FALSE,
                     camera->projection_matrix);

  glUniformMatrix4fv(world->shader.view_matrix_uniform, 1, GL_FALSE,
                     camera->view_matrix);

  glEnableVertexAttribArray(world->shader.vertex_position_attribute);
  glEnableVertexAttribArray(world->shader.vertex_normal_attribute);

  for (unsigned int x = 0; x < render_distance; x++) {
    for (unsigned int y = 0; y < render_distance; y++) {
      for (unsigned int z = 0; z < render_distance; z++) {
        Chunk *chunk = world->chunks[x][y][z];

        bool chunk_locked = mtx_trylock(&chunk->mutex);
        if (chunk_locked) {
          continue;
        }

        if (chunk->mesh_size == 0) {
          mtx_unlock(&chunk->mutex);
          continue;
        }

        glUniform3f(world->shader.chunk_position_uniform, chunk->position[0],
                    chunk->position[1], chunk->position[2]);

        glBindBuffer(GL_ARRAY_BUFFER, chunk->vertex_buffer);
        glVertexAttribPointer(world->shader.vertex_position_attribute, 1,
                              GL_FLOAT, GL_FALSE, 0, NULL);
        glBindBuffer(GL_ARRAY_BUFFER, chunk->normal_buffer);
        glVertexAttribPointer(world->shader.vertex_normal_attribute, 1,
                              GL_FLOAT, GL_FALSE, 0, NULL);

        glDrawArrays(GL_TRIANGLES, 0, chunk->mesh_size);
        mtx_unlock(&chunk->mutex);
      }
    }
  }

  glDisableVertexAttribArray(world->shader.vertex_position_attribute);
  glDisableVertexAttribArray(world->shader.vertex_normal_attribute);
}

void world_free(World *world) {
  for (unsigned int x = 0; x < render_distance; x++) {
    for (unsigned int y = 0; y < render_distance; y++) {
      for (unsigned int z = 0; z < render_distance; z++) {
        Chunk *chunk = world->chunks[x][y][z];
        if (chunk == NULL) {
          continue;
        }

        free(chunk);
      }
    }
  }

  free(world->chunk_thread_data.chunks);
  free(world->chunk_thread_data.out);
}

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "camera.h"
#include "chunk.h"
#include "load_shader.h"
#include "mat4.h"
#include <stdio.h>

int main() {
  GLFWwindow *window;

  if (!glfwInit()) {
    return 1;
  }

  window = glfwCreateWindow(1920, 1080, "voxel", NULL, NULL);

  if (!window) {
    glfwTerminate();
    return 1;
  }

  glfwWindowHint(GLFW_SAMPLES, 4);

  glfwMakeContextCurrent(window);

  if (glewInit() != GLEW_OK) {
    glfwTerminate();
    return 1;
  }

  glClearColor(0.2, 0.5, 0.9, 1.0);

  unsigned int vertex_array_object;
  glGenVertexArrays(1, &vertex_array_object);
  glBindVertexArray(vertex_array_object);

  unsigned int program_id = load_shader("assets/shaders/vertex_shader.glsl",
                                        "assets/shaders/fragment_shader.glsl");

  unsigned int vertex_position_attribute =
      glGetAttribLocation(program_id, "vertex_position");
  unsigned int vertex_normal_attribute =
      glGetAttribLocation(program_id, "vertex_normal");

  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  Camera camera = {.transform = {.position = {0, 0, 0},
                                 .rotation = {0, 0, 0},
                                 .scale = {1, 1, 1}},
                   .speed = 0.5,
                   .sensitivity = 20};

  mat4_create_projection_matrix(camera.projection_matrix, 75, 16.0 / 9, 0.1,
                                100);
  mat4_create_identity_matrix(camera.view_matrix);

  unsigned int projection_matrix_uniform =
      glGetUniformLocation(program_id, "projection_matrix");

  unsigned int view_matrix_uniform =
      glGetUniformLocation(program_id, "view_matrix");

  Chunk chunk = {0};
  chunk_init(&chunk);
  chunk_build_mesh(&chunk);

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glEnable(GL_MULTISAMPLE);

  while (!glfwWindowShouldClose(window)) {
    camera_move(&camera, window);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(program_id);

    glUniformMatrix4fv(projection_matrix_uniform, 1, GL_FALSE,
                       camera.projection_matrix);

    mat4_from_transform(camera.view_matrix, &camera.transform);
    mat4_inverse(camera.view_matrix, camera.view_matrix);

    glUniformMatrix4fv(view_matrix_uniform, 1, GL_FALSE, camera.view_matrix);

    glBindBuffer(GL_ARRAY_BUFFER, chunk.vertex_buffer);
    glEnableVertexAttribArray(vertex_position_attribute);
    glVertexAttribPointer(vertex_position_attribute, 1, GL_FLOAT, GL_FALSE, 0,
                          NULL);
    glBindBuffer(GL_ARRAY_BUFFER, chunk.normal_buffer);
    glEnableVertexAttribArray(vertex_normal_attribute);
    glVertexAttribPointer(vertex_normal_attribute, 1, GL_FLOAT, GL_FALSE, 0,
                          NULL);

    glDrawArrays(GL_TRIANGLES, 0, chunk.mesh_size);

    glDisableVertexAttribArray(vertex_position_attribute);
    glDisableVertexAttribArray(vertex_normal_attribute);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();

  chunk_free(&chunk);

  return 0;
}

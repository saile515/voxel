#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "../tracy/public/tracy/TracyC.h"
#include "camera.h"
#include "mat4.h"
#include "world.h"
#include <stdio.h>
#include <time.h>

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

  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  Camera camera = {.transform = {.position = {0, 0, 0},
                                 .rotation = {0, 0, 0},
                                 .scale = {1, 1, 1}},
                   .speed = 0.5,
                   .sensitivity = 20};

  mat4_create_projection_matrix(camera.projection_matrix, 75, 16.0 / 9, 0.1,
                                1000);
  mat4_create_identity_matrix(camera.view_matrix);

  World world = {0};
  world_init(&world);

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glEnable(GL_MULTISAMPLE);

  while (!glfwWindowShouldClose(window)) {
    camera_move(&camera, window);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    world_load(&world, &camera);
    world_render(&world, &camera);

    TracyCFrameMark;

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();

  world_free(&world);

  return 0;
}

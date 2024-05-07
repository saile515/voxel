#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "load_shader.h"

static const float vertices[] = {-0.5, -0.5, 0.0, -0.5, 0.5,  0.0,
                                 0.5,  0.5,  0.0, 0.5,  -0.5, 0.0};
static const unsigned int indices[] = {1, 0, 2, 2, 0, 3};

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

  glfwMakeContextCurrent(window);

  if (glewInit() != GLEW_OK) {
    glfwTerminate();
    return 1;
  }

  glClearColor(0.0, 0.0, 0.1, 1.0);

  unsigned int vertex_array_object;
  glGenVertexArrays(1, &vertex_array_object);
  glBindVertexArray(vertex_array_object);

  unsigned int vertex_buffer;
  glGenBuffers(1, &vertex_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices[0], GL_STATIC_DRAW);

  unsigned int index_buffer;
  glGenBuffers(1, &index_buffer);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), &indices[0],
               GL_STATIC_DRAW);

  unsigned int program_id = load_shader("assets/shaders/vertex_shader.glsl",
                                        "assets/shaders/fragment_shader.glsl");

  while (!glfwWindowShouldClose(window)) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(program_id);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
    glDrawElements(GL_TRIANGLE_STRIP, sizeof(indices) / sizeof(unsigned int),
                   GL_UNSIGNED_INT, 0);

    glDisableVertexAttribArray(0);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();

  return 0;
}

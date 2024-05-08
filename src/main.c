#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "load_shader.h"
#include "mat4.h"
#include "vec3.h"

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
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  unsigned int index_buffer;
  glGenBuffers(1, &index_buffer);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
               GL_STATIC_DRAW);

  unsigned int program_id = load_shader("assets/shaders/vertex_shader.glsl",
                                        "assets/shaders/fragment_shader.glsl");

  unsigned int vertex_position_attribute =
      glGetAttribLocation(program_id, "vertex_position");

  Mat4 projection_matrix =
      mat4_create_projection_matrix(75, 16.0 / 9, 0.1, 100);
  unsigned int projection_matrix_uniform =
      glGetUniformLocation(program_id, "projection_matrix");
  mat4_print(&projection_matrix);

  Mat4 view_matrix = mat4_create_identity_matrix();
  unsigned int view_matrix_uniform =
      glGetUniformLocation(program_id, "view_matrix");

  view_matrix = mat4_translate(&view_matrix, (Vec3){{0, 0, -10}});

  while (!glfwWindowShouldClose(window)) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(program_id);

    glEnableVertexAttribArray(vertex_position_attribute);
    glVertexAttribPointer(vertex_position_attribute, 3, GL_FLOAT, GL_FALSE, 0,
                          NULL);

    glUniformMatrix4fv(projection_matrix_uniform, 1, GL_FALSE,
                       projection_matrix.values);

    glUniformMatrix4fv(view_matrix_uniform, 1, GL_FALSE, view_matrix.values);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
    glDrawElements(GL_TRIANGLE_STRIP, sizeof(indices) / sizeof(unsigned int),
                   GL_UNSIGNED_INT, 0);

    glDisableVertexAttribArray(vertex_position_attribute);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();

  return 0;
}

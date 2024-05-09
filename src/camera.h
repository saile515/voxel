#pragma once

#include "mat4.h"
#include "transform.h"
#include <GLFW/glfw3.h>

typedef struct Camera {
  Transform transform;
  double speed;
  double sensitivity;
  Mat4 projection_matrix;
  Mat4 view_matrix;
} Camera;

void camera_move(Camera *camera, GLFWwindow *window);

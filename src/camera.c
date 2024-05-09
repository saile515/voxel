#include "camera.h"

#include "vec2.h"
#include "vec3.h"
#include <math.h>

static const double DEG_TO_RAD = M_PI / 180;

void camera_move(Camera *camera, GLFWwindow *window) {
  Vec2i window_size;
  Vec2 cursor_position;

  glfwGetWindowSize(window, &window_size[0], &window_size[1]);

  glfwGetCursorPos(window, &cursor_position[0], &cursor_position[1]);
  camera->transform.rotation[1] =
      -cursor_position[0] / window_size[0] * camera->sensitivity;
  camera->transform.rotation[0] =
      -cursor_position[1] / window_size[0] * camera->sensitivity;

  Vec2 local_direction = {0};

  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
    local_direction[1] -= 1;
  }

  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
    local_direction[0] -= 1;
  }

  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
    local_direction[1] += 1;
  }

  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
    local_direction[0] += 1;
  }

  double yaw = camera->transform.rotation[1] * DEG_TO_RAD;
  double pitch = camera->transform.rotation[0] * DEG_TO_RAD;

  Vec3 forward;

  forward[0] = sin(yaw) * cos(pitch);
  forward[1] = -sin(pitch);
  forward[2] = cos(yaw) * cos(pitch);

  Vec3 right;

  right[0] = cos(yaw);
  right[1] = 0;
  right[2] = -sin(yaw);

  vec3_multiply_double(forward, forward, local_direction[1]);
  vec3_multiply_double(right, right, local_direction[0]);

  Vec3 move_direction;

  vec3_add(move_direction, forward, right);
  vec3_normalize(move_direction, move_direction);
  vec3_multiply_double(move_direction, move_direction, camera->speed);

  vec3_add(camera->transform.position, camera->transform.position,
           move_direction);
}

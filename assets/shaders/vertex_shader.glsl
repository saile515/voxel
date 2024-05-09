#version 450 core

uniform mat4 projection_matrix;
uniform mat4 view_matrix;
uniform mat4 model_matrix;

in vec3 vertex_position;

void main() {
  gl_Position = projection_matrix * view_matrix * model_matrix * vec4(vertex_position, 1.0);
}

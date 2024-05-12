#version 450 core

uniform mat4 projection_matrix;
uniform mat4 view_matrix;

in float vertex_position;
out vec3 vert;

void main() {
  vert[0] = round(mod(vertex_position, 33));
  vert[1] = floor(mod(vertex_position / 33, 33));
  vert[2] = floor(vertex_position / (33 * 33));

  gl_Position = projection_matrix * view_matrix * vec4(vert, 1.0);
}

#version 450 core

uniform mat4 projection_matrix;
uniform mat4 view_matrix;
uniform vec3 chunk_position;

in float vertex_position;
in float vertex_normal;
out vec3 pos;
out float norm;

void main() {
  pos[0] = round(mod(vertex_position, 33));
  pos[1] = floor(mod(vertex_position / 33, 33));
  pos[2] = floor(vertex_position / (33 * 33));

  norm = vertex_normal;

  gl_Position = projection_matrix * view_matrix * vec4(pos + chunk_position * 32, 1.0);
}

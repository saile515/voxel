#version 450 core

out vec4 color;
in vec3 vert;

void main() {
  color = vec4(vert.x / 33, vert.y / 33, vert.z / 33, 1.0);
}

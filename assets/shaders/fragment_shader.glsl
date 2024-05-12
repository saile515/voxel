#version 450 core

out vec3 color;
in vec3 vert;

void main() {
  color = vec3(vert.x / 33, vert.y / 33, vert.z / 33);
}

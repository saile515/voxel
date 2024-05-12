#version 450 core

in vec3 pos;
in float norm;
out vec3 color;

void main() {
  color = vec3(pos.x / 33, pos.y / 33, pos.z / 33);
  color *= (norm + 5) / 10;
}

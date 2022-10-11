#version 330 core

struct Material {
  vec3 color;
};
uniform Material material;
out vec3 color;

void main() {
  color = material.color;
}
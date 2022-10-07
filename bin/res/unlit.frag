#version 330 core

uniform vec3 objectColor;
out vec3 color;

void main() {
  color = objectColor;
}
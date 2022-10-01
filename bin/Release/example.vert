#version 330 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 vertexColor;

uniform mat4 transform;
out vec3 fragmentColor;

void main() {
  gl_Position =  transform * vec4(pos, 1);
  fragmentColor = vertexColor;
}
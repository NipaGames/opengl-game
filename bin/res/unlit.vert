#version 330 core

layout(location = 0) in vec3 pos;

uniform mat4 transform;
uniform vec3 vertexColor;
out vec3 fragmentColor;

void main() {
  gl_Position =  transform * vec4(pos, 1);
  fragmentColor = vertexColor;
}
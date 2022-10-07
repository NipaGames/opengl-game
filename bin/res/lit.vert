#version 330 core

out vec3 fragmentNormal;
out vec3 fragmentPos;

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 normal;

uniform mat4 viewProjection;
uniform mat4 model;

void main() {
  gl_Position = viewProjection * model * vec4(pos, 1);
  fragmentPos = vec3(model * vec4(pos, 1));
  fragmentNormal = mat3(transpose(inverse(model))) * normal;
}
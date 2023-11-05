#version 330 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in vec3 normal;

out vec3 fragmentNormal;
out vec3 fragmentPos;
out vec2 fragmentTexCoord;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main() {
  gl_Position = projection * view * model * vec4(pos, 1);
  fragmentPos = vec3(model * vec4(pos, 1));
  fragmentNormal = mat3(transpose(inverse(model))) * normal;
  fragmentTexCoord = texCoord;
}
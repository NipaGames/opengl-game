#version 330 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in vec3 normal;

out vec3 fragmentNormal;
out vec3 fragmentPos;
out vec2 fragmentTexCoord;
out vec3 fragmentViewPos;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 hudView;
uniform mat4 model;
uniform vec3 viewPos;

void main() {
  gl_Position = projection * hudView * model * vec4(pos, 1.0);
  fragmentPos = vec3(inverse(view) * vec4(pos, 1));
  fragmentNormal = mat3(transpose(view * model)) * normal;
  fragmentTexCoord = texCoord;
  fragmentViewPos = -viewPos + 2.0 * fragmentPos;
}
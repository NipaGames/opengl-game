#version 330 core

layout(location = 0) in vec3 pos;

uniform mat4 transform;
uniform vec3 vertexColor;
uniform vec3 lightColor = vec3(1.0, 1.0, 1.0);
out vec3 fragmentColor;

void main() {
  gl_Position =  transform * vec4(pos, 1);
  fragmentColor = lightColor * vertexColor;
}
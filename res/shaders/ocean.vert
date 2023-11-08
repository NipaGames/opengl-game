#version 330 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 texCoord;

out vec2 fragmentTexCoord;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

uniform float time;

float rand(vec2 seed){
    return fract(sin(dot(seed, vec2(12.9898, 78.233))) * 43758.5453);
}

void main() {
  float waveHeight = 1.5;
  float waveSpeed = 0.25;

  float randFactor = rand(pos.xz);
  float offset = sin(randFactor + time * waveSpeed * randFactor * radians(360.0)) * waveHeight;
  vec3 offsetPos = pos;
  offsetPos.y += offset;
  gl_Position =  projection * view * model * vec4(offsetPos, 1);
  fragmentTexCoord = texCoord;
}
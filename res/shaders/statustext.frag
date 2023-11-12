#version 330 core

out vec4 color;

in vec2 fragmentTexCoord;

uniform vec4 textColor;
uniform sampler2D textureSampler;

uniform float time;

void main() {
  float speed = 1.0;
  vec4 sampled = vec4(1.0, 1.0, 1.0, texture(textureSampler, fragmentTexCoord).r * (cos(time * speed * radians(360.0)) + 1.0) / 2.0);
  color = sampled * textColor;
}
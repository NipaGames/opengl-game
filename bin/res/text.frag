#version 330 core

out vec4 color;

in vec2 fragmentTexCoord;

uniform vec4 textColor;
uniform sampler2D textureSampler;

void main() {
  vec4 sampled = vec4(1.0, 1.0, 1.0, texture(textureSampler, fragmentTexCoord).r);
  color = sampled * textColor;
}
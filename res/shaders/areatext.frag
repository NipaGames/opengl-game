#version 330 core

out vec4 color;

in vec2 fragmentTexCoord;

uniform vec4 textColor;
uniform sampler2D textureSampler;
uniform float time;

void main() {
  float speed = 0.25;
  float waviness = 8.0;
  float glint = 0.1;
  float amt = 0.01;

  vec2 pos = fragmentTexCoord;
  pos.x += (sin((time * radians(360.0)) * speed + pos.y * waviness) + 1.0) / 2.0 * amt;
  pos.x -= amt;
  pos.x /= 1.0 - amt;
  vec4 sampled = vec4(1.0, 1.0, 1.0, texture(textureSampler, pos).r);
  float glintModifier = (sin(time / 2.0 * radians(360.0) + (1.0 - pos.x) * radians(180.0)) + 1.0) * pow(1.0 - abs(2.0 * pos.y - 1.0), 2.0) * glint;
  vec4 glintColor;
  glintColor.xyz = vec3(glintModifier);
  glintColor.w = glintModifier * (1.0 - abs(2.0 * pos.x - 1.0));
  color = (sampled + glintColor) * textColor;
}
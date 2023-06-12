#version 330 core

#define MAX_COLORS 8

struct Material {
  vec3[MAX_COLORS] colors;
  int colorCount;
  float strobeInterval;
  bool hasTexture;
};

out vec4 color;

in vec2 fragmentTexCoord;

uniform Material material;
uniform sampler2D textureSampler;
uniform float time;

void main() {
  int currentColor = int(fract(time / material.strobeInterval / material.colorCount) * material.colorCount);
  color = vec4(material.colors[currentColor], 1.0);
  if (material.hasTexture)
    color *= texture(textureSampler, fragmentTexCoord);
}
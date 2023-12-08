#version 330 core

struct Material {
  vec4 color;
  bool hasTexture;
  vec2 tiling;
  vec2 offset;
};

in vec2 fragmentTexCoord;

out vec4 color;

uniform Material material;
uniform sampler2D textureSampler;

void main() {
  vec4 col = material.color;
  if (material.hasTexture)
    col *= texture(textureSampler, fragmentTexCoord * material.tiling + material.offset);
  color = col;
}
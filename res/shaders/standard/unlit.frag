#version 330 core

struct Material {
  vec3 color;
  vec3 tint;
  float opacity;
  bool hasTexture;
  vec2 tiling;
  vec2 offset;
};

out vec4 color;

in vec2 fragmentTexCoord;

uniform Material material;
uniform sampler2D textureSampler;

void main() {
  vec3 col = material.color;
  if (material.hasTexture)
    col *= texture(textureSampler, fragmentTexCoord * material.tiling + material.offset);
  col += material.tint;
  color = vec4(col, material.opacity);
}
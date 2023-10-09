#version 330 core

struct Material {
  vec3 color;
  float opacity;
  bool hasTexture;
  vec2 tiling;
};

out vec4 color;

in vec2 fragmentTexCoord;

uniform Material material;
uniform sampler2D textureSampler;

void main() {
  color = vec4(material.color, material.opacity);
  if (material.hasTexture)
    color *= texture(textureSampler, fragmentTexCoord * material.tiling);
}
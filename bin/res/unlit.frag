#version 330 core

struct Material {
  vec3 color;
  bool hasTexture;
};

out vec3 color;

in vec2 fragmentTexCoord;

uniform Material material;
uniform sampler2D textureSampler;

void main() {
  color = material.color;
  if (material.hasTexture)
    color *= texture(textureSampler, fragmentTexCoord).xyz;
}
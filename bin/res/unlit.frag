#version 330 core

struct Material {
  vec3 color;
  bool hasTexture;
};

out vec4 color;

in vec2 fragmentTexCoord;

uniform Material material;
uniform sampler2D textureSampler;

void main() {
  color = vec4(material.color, 1.0);
  if (material.hasTexture)
    color *= texture(textureSampler, fragmentTexCoord);
}
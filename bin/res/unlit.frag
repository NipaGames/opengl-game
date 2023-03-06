#version 330 core

struct Material {
  vec3 color;
};

out vec3 color;

in vec2 fragmentTexCoord;

uniform Material material;
uniform sampler2D textureSampler;

void main() {
  color = texture(textureSampler, fragmentTexCoord).xyz * material.color;
}
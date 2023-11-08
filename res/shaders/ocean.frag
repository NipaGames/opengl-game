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
uniform float time;

void main() {
  float speed = 0.25;
  float waviness = 64.0;
  vec2 amt = vec2(0.005, 0.001);

  vec2 pos = fragmentTexCoord;
  pos.x += (sin((time / 5.0 * radians(360.0)) * speed + pos.y * waviness) + 1.0) / 2.0 * amt.x;
  pos.x -= amt.x;
  pos.x /= 1.0 - amt.x;

  pos.y += (sin((time * radians(360.0)) * speed + pos.x * waviness) + 1.0) / 2.0 * amt.y;
  pos.y -= amt.y;
  pos.y /= 1.0 - amt.y;

  color = vec4(material.color, material.opacity);
  if (material.hasTexture)
    color *= texture(textureSampler, pos * material.tiling);
}
#version 330 core

struct Material {
  vec3 color;
  float opacity;
  bool hasTexture;
  vec2 tiling;
  vec2 offset;
};

out vec4 color;

in vec2 fragmentTexCoord;

uniform Material material;
uniform sampler2D textureSampler;
uniform float time;

float near = 0.1; 
float far = 200.0;
vec3 fogColor = vec3(0.2, 0.2, 0.25);

// copied from learnopengl.com
float linearizeDepth(float depth) {
  float z = depth * 2.0 - 1.0; // back to NDC 
  return (2.0 * near * far) / (far + near - z * (far - near));	
}

void main() {
  vec2 speed = vec2(0.05, 0.25);
  vec2 amt = vec2(0.5, 0.1);
  float waviness = 0.5;

  vec2 pos = fragmentTexCoord;
  pos.x += (sin((time * radians(360.0)) * speed.x + pos.y * waviness) + 1.0) / 2.0 * amt.x;
  pos.x -= amt.x;
  pos.x /= 1.0 - amt.x;

  pos.y += (sin((time * radians(360.0)) * speed.y + pos.x * waviness) + 1.0) / 2.0 * amt.y;
  pos.y -= amt.y;
  pos.y /= 1.0 - amt.y;

  color = vec4(material.color, material.opacity);
  if (material.hasTexture)
    color *= texture(textureSampler, pos * material.tiling + material.offset);

  float fog = linearizeDepth(gl_FragCoord.z) / far;
  color *= vec4(vec3(1.0 - fog), 1.0);
  color += vec4(fog * fogColor, 1.0);
}
#version 330 core

struct Fog {
  bool use;
  float near;
  float far;
  vec3 color;
};

struct Material {
  vec3 color;
  vec3 tint;
  float opacity;
  bool hasTexture;
  vec2 tiling;
  vec2 offset;
  Fog fog;
};

out vec4 color;

in vec2 fragmentTexCoord;

uniform Material material;
uniform sampler2D textureSampler;

// copied from learnopengl.com
float linearizeDepth(float depth, float near, float far) {
  float z = depth * 2.0 - 1.0; // back to NDC 
  return (2.0 * near * far) / (far + near - z * (far - near));	
}

void main() {
  vec4 col = vec4(material.color, material.opacity);
  if (material.hasTexture)
    col *= texture(textureSampler, fragmentTexCoord * material.tiling + material.offset);
  col.rgb += material.tint;
  if (material.fog.use) {
    float fog = linearizeDepth(gl_FragCoord.z, material.fog.near, material.fog.far) / material.fog.far;
    col.rgb *= vec3(1.0 - fog);
    col.rgb += fog * material.fog.color;
  }
  color = col;
}
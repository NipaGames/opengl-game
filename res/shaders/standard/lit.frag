#version 330 core
out vec4 color;

#define LIGHT_NONE 0
#define LIGHT_POINT_LIGHT 1
#define LIGHT_SPOTLIGHT 2
#define LIGHT_DIRECTIONAL_LIGHT 3
#define LIGHT_DIRECTIONAL_LIGHT_PLANE 4

struct Light {
  bool enabled;
  vec3 color;
  float intensity;
  int type;

  vec3 pos;
  float range;
  vec3 dir;
  float y;
  float cutOffMin;
  float cutOffMax;
};

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
  float specularStrength;
  int specularHighlight;
  vec3 ambientColor;
  bool hasTexture;
  vec2 tiling;
  vec2 offset;
  Fog fog;
};

in vec3 fragmentNormal;
in vec3 fragmentPos;
in vec2 fragmentTexCoord;
in vec3 fragmentViewPos;

#define MAX_LIGHTS 32
uniform Light[MAX_LIGHTS] lights;

uniform Material material;
uniform sampler2D textureSampler;

vec3 dirLight(vec3 lightDir, vec3 lightColor, vec3 normal) {
  vec3 diffuse = max(dot(normal, lightDir), 0.0) * lightColor;
  vec3 viewDir = normalize(fragmentViewPos - fragmentPos);
  vec3 reflectDir = reflect(-lightDir, normal);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.specularHighlight);
  vec3 specular = material.specularStrength * spec * lightColor;
  return diffuse + specular;
}

vec3 calcDirectionalLight(Light light, vec3 normal) {
  return dirLight(light.dir, light.color, normal) * light.intensity;
}

vec3 calcDirectionalLightPlane(Light light, vec3 normal) {
  return (dirLight(light.dir, light.color, normal) * light.intensity) / max(1.0, pow(abs(distance(fragmentPos.y, light.y)) / light.range, 2));
}

vec3 calcPointLight(Light light, vec3 normal) {
  float dist = distance(fragmentPos, light.pos);
  if (dist > light.range)
    return vec3(0.0);
  vec3 lightDir = normalize(light.pos - fragmentPos);
  return (dirLight(lightDir, light.color, normal) * light.intensity) * pow(1.0 - dist / light.range, 2);
}

vec3 calcSpotlight(Light light, vec3 normal) {
  vec3 lightDir = normalize(light.pos - fragmentPos);
  float theta = dot(lightDir, normalize(-light.dir));
  if(theta > light.cutOffMax) {
    Light pointLight = light;
    pointLight.intensity = clamp((theta - light.cutOffMax) / (light.cutOffMin - light.cutOffMax), 0.0, 1.0) * light.intensity;
    return calcPointLight(pointLight, normal);
  }
  else
    return vec3(0.0);
}


// copied from learnopengl.com
float linearizeDepth(float depth, float near, float far) {
  float z = depth * 2.0 - 1.0; // back to NDC 
  return (2.0 * near * far) / (far + near - z * (far - near));	
}

void main() {
  vec3 normal = normalize(fragmentNormal);
  vec3 col = vec3(0.0);
  vec3 materialColor = material.color + material.tint;

  for(int i = 0; i < lights.length(); i++) {
    if (!lights[i].enabled)
      continue;
    switch (lights[i].type) {
      case LIGHT_POINT_LIGHT:
        col += calcPointLight(lights[i], normal) * material.color;
        break;
      case LIGHT_SPOTLIGHT:
        col += calcSpotlight(lights[i], normal) * material.color;
        break;
      case LIGHT_DIRECTIONAL_LIGHT:
        col += calcDirectionalLight(lights[i], normal) * material.color;
        break;
      case LIGHT_DIRECTIONAL_LIGHT_PLANE:
        col += calcDirectionalLightPlane(lights[i], normal) * material.color;
        break;
      case LIGHT_NONE:
      default:
        break;
    }
  }
  // more advanced ambient lighting (not necessary):
  // col += max(dot(normal, vec3(0.0, 1.0, 0.0)), length(material.ambientColor)) * material.ambientColor * material.color;
  col += material.ambientColor;
  if (material.hasTexture)
    col *= texture(textureSampler, fragmentTexCoord * material.tiling + material.offset).xyz;
  col += material.tint;

  if (material.fog.use) {
    float fog = linearizeDepth(gl_FragCoord.z, material.fog.near, material.fog.far) / material.fog.far;
    col *= vec3(1.0 - fog);
    col += fog * material.fog.color;
  }
  color = vec4(col, material.opacity);
}
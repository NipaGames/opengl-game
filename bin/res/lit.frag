#version 330 core
out vec3 color;

struct PointLight {
  vec3 pos;
  vec3 color;
  float range;
  float intensity;
};

struct Spotlight {
  vec3 pos;
  vec3 dir;
  float cutOffMin;
  float cutOffMax;
  vec3 color;
  float range;
  float intensity;
};

struct Material {
  vec3 color;
  float specularStrength;
  int specularHighlight;
  vec3 ambientColor;
};

in vec3 fragmentNormal;
in vec3 fragmentPos;

#define MAX_POINT_LIGHTS 8
uniform PointLight[MAX_POINT_LIGHTS] pointLights;
#define MAX_SPOT_LIGHTS 8
uniform Spotlight[MAX_SPOT_LIGHTS] spotlights;
uniform Material material;
uniform vec3 viewPos;

vec3 calcPointLight(PointLight light, vec3 normal) {
  vec3 lightDir = normalize(light.pos - fragmentPos);
  vec3 diffuse = max(dot(normal, lightDir), 0.0) * light.color;
  vec3 viewDir = normalize(viewPos - fragmentPos);
  vec3 reflectDir = reflect(-lightDir, normal);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.specularHighlight);
  vec3 specular = material.specularStrength * spec * light.color;
  return ((diffuse + specular) * light.intensity) / max(1.0, pow(abs(distance(fragmentPos, light.pos)) / light.range, 2)) * material.color;
}

vec3 calcSpotlight(Spotlight light, vec3 normal) {
  vec3 lightDir = normalize(light.pos - fragmentPos);
  float theta = dot(lightDir, normalize(-light.dir));
  if(theta > light.cutOffMax) {
    PointLight light2;
    light2.pos = light.pos;
    light2.color = light.color;
    light2.range = light.range;
    light2.intensity = clamp((theta - light.cutOffMax) / (light.cutOffMin - light.cutOffMax), 0.0, 1.0) * light.intensity;
    return calcPointLight(light2, normal);
  }
  else
    return vec3(0.0, 0.0, 0.0);
}

void main() {
  vec3 normal = normalize(fragmentNormal);
  color = vec3(0.0);

  for(int i = 0; i < pointLights.length(); i++) {
    color += calcPointLight(pointLights[i], normal);
  }
  for(int i = 0; i < spotlights.length(); i++) {
    color += calcSpotlight(spotlights[i], normal);
  }
  color += max(dot(normal, normalize(fragmentPos)), 0.0) * material.ambientColor;
}
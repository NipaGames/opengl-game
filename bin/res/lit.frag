#version 330 core
out vec3 color;

struct PointLight {
  vec3 pos;
  vec3 color;
  float range;
  float intensity;
};

struct DirectionalLight {
  vec3 dir;
  vec3 color;
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
#define MAX_DIRECTIONAL_LIGHTS 8
uniform DirectionalLight[MAX_DIRECTIONAL_LIGHTS] directionalLights;
#define MAX_SPOT_LIGHTS 8
uniform Spotlight[MAX_SPOT_LIGHTS] spotlights;
uniform Material material;
uniform vec3 viewPos;

vec3 dirLight(vec3 lightDir, vec3 lightColor, vec3 normal) {
  vec3 diffuse = max(dot(normal, lightDir), 0.0) * lightColor;
  vec3 viewDir = normalize(viewPos - fragmentPos);
  vec3 reflectDir = reflect(-lightDir, normal);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.specularHighlight);
  vec3 specular = material.specularStrength * spec * lightColor;
  return diffuse + specular;
}

vec3 calcDirectionalLight(DirectionalLight light, vec3 normal) {
  return (dirLight(light.dir, light.color, normal) * light.intensity) * material.color;
}

vec3 calcPointLight(PointLight light, vec3 normal) {
  vec3 lightDir = normalize(light.pos - fragmentPos);
  return (dirLight(lightDir, light.color, normal) * light.intensity) / max(1.0, pow(abs(distance(fragmentPos, light.pos)) / light.range, 2)) * material.color;
}

vec3 calcSpotlight(Spotlight light, vec3 normal) {
  vec3 lightDir = normalize(light.pos - fragmentPos);
  float theta = dot(lightDir, normalize(-light.dir));
  if(theta > light.cutOffMax) {
    PointLight pointLight;
    pointLight.pos = light.pos;
    pointLight.color = light.color;
    pointLight.range = light.range;
    pointLight.intensity = clamp((theta - light.cutOffMax) / (light.cutOffMin - light.cutOffMax), 0.0, 1.0) * light.intensity;
    return calcPointLight(pointLight, normal);
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
  for(int i = 0; i < directionalLights.length(); i++) {
    color += calcDirectionalLight(directionalLights[i], normal);
  }
  for(int i = 0; i < spotlights.length(); i++) {
    color += calcSpotlight(spotlights[i], normal);
  }
  // more advanced ambient lighting (not necessary):
  // color += max(dot(normal, vec3(0.0, 1.0, 0.0)), length(material.ambientColor)) * material.ambientColor * material.color;
  color += material.ambientColor;
}
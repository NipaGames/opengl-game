#version 330 core
out vec4 color;

struct PointLight {
  bool enabled;
  vec3 pos;
  vec3 color;
  float range;
  float intensity;
};

struct DirectionalLight {
  bool enabled;
  vec3 dir;
  vec3 color;
  float intensity;
};

struct DirectionalLightPlane {
  bool enabled;
  vec3 dir;
  vec3 color;
  float intensity;
  float y;
  float range;
};

struct Spotlight {
  bool enabled;
  vec3 pos;
  vec3 dir;
  float cutOffMin;
  float cutOffMax;
  vec3 color;
  float range;
  float intensity;
};

struct Fog {
  bool use;
  float near;// = 0.1;
  float far;// = 50.0;
  vec3 color;// = vec3(0.075, 0.035, 0.025);
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

#define MAX_POINT_LIGHTS 8
uniform PointLight[MAX_POINT_LIGHTS] pointLights;
#define MAX_DIRECTIONAL_LIGHTS 8
uniform DirectionalLight[MAX_DIRECTIONAL_LIGHTS] directionalLights;
#define MAX_DIRECTIONAL_LIGHT_PLANES 8
uniform DirectionalLightPlane[MAX_DIRECTIONAL_LIGHT_PLANES] directionalLightPlanes;
#define MAX_SPOT_LIGHTS 8
uniform Spotlight[MAX_SPOT_LIGHTS] spotlights;
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

vec3 calcDirectionalLight(DirectionalLight light, vec3 normal) {
  return dirLight(light.dir, light.color, normal) * light.intensity;
}

vec3 calcDirectionalLightPlane(DirectionalLightPlane light, vec3 normal) {
  return (dirLight(light.dir, light.color, normal) * light.intensity) / max(1.0, pow(abs(distance(fragmentPos.y, light.y)) / light.range, 2));
}

vec3 calcPointLight(PointLight light, vec3 normal) {
  vec3 lightDir = normalize(light.pos - fragmentPos);
  return (dirLight(lightDir, light.color, normal) * light.intensity) / max(1.0, pow(abs(distance(fragmentPos, light.pos)) / light.range, 2));
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

  for(int i = 0; i < pointLights.length(); i++) {
    if (pointLights[i].enabled == true)
      col += calcPointLight(pointLights[i], normal) * material.color;
  }
  for(int i = 0; i < directionalLights.length(); i++) {
    if (directionalLights[i].enabled == true)
      col += calcDirectionalLight(directionalLights[i], normal) * material.color;
  }
  for(int i = 0; i < directionalLightPlanes.length(); i++) {
    if (directionalLightPlanes[i].enabled == true)
      col += calcDirectionalLightPlane(directionalLightPlanes[i], normal) * material.color;
  }
  for(int i = 0; i < spotlights.length(); i++) {
    if (spotlights[i].enabled == true)
      col += calcSpotlight(spotlights[i], normal) * material.color;
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
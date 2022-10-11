#version 330 core
out vec3 color;

struct Light {
  vec3 pos;
  vec3 color;
  float range;
  float intensity;
};

in vec3 fragmentNormal;
in vec3 fragmentPos;

uniform vec3 objectColor;
uniform Light light;
uniform vec3 viewPos;
uniform float specularStrength = .5;
uniform int specularHighlight = 32;

void main() {
  float ambientStrength = .25;

  vec3 normal = normalize(fragmentNormal);
  vec3 lightDir = normalize(light.pos - fragmentPos);

  vec3 ambient = ambientStrength * light.color;
  vec3 diffuse = max(dot(normal, lightDir), 0.0) * light.color;
  vec3 viewDir = normalize(viewPos - fragmentPos);
  vec3 reflectDir = reflect(-lightDir, normal);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), specularHighlight);
  vec3 specular = specularStrength * spec * light.color;

  color = ambient + ((diffuse + specular) * light.intensity) / max(1.0, pow(abs(distance(fragmentPos, light.pos)) / light.range, 2)) * objectColor;
}
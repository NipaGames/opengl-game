#version 330 core
out vec3 color;

in vec3 fragmentNormal;
in vec3 fragmentPos;

uniform vec3 objectColor;
uniform vec3 lightPos = vec3(0.0, 0.0, 0.0);
uniform vec3 lightColor = vec3(1.0, 1.0, 1.0);
uniform float lightRange = 1.0;
uniform float lightIntensity = 1.0;
uniform vec3 viewPos;
uniform float specularStrength = .5;
uniform int specularHighlight = 32;

void main() {
  float ambientStrength = .25;

  vec3 normal = normalize(fragmentNormal);
  vec3 lightDir = normalize(lightPos - fragmentPos);
  float cosTheta = clamp(dot(normal, lightDir), 0, 1);

  vec3 ambient = ambientStrength * lightColor;
  vec3 diffuse = cosTheta * lightColor;
  vec3 viewDir = normalize(viewPos - fragmentPos);
  vec3 reflectDir = reflect(-lightDir, normal);
  float spec = pow(cosTheta, specularHighlight);
  vec3 specular = specularStrength * spec * lightColor;

  color = ambient + ((diffuse + specular) * lightIntensity) / max(1.0, pow(abs(distance(fragmentPos, lightPos)) / lightRange, 2)) * objectColor;
}
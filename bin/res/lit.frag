#version 330 core
out vec3 color;

in vec3 fragmentNormal;
in vec3 fragmentPos;

uniform vec3 objectColor;
uniform vec3 lightPos = vec3(0.0, 0.0, 0.0);
uniform vec3 lightColor = vec3(1.0, 1.0, 1.0);

void main() {
  float ambientStrength = .15f;
  vec3 ambient = ambientStrength * lightColor;
  vec3 normal = normalize(fragmentNormal);
  vec3 lightDir = normalize(lightPos - fragmentPos);
  float diff = max(dot(normal, lightDir), 0.0);
  vec3 diffuse = (diff * lightColor) / abs(distance(fragmentPos, lightPos));
  color = (ambient + diffuse) * objectColor;
}
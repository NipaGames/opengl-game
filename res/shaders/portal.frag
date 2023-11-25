#version 330 core

out vec4 color;

in vec2 fragmentTexCoord;
uniform float time;

void main() {
  float waves = 5;
  float speed = 0.25;

  float dist = distance(fragmentTexCoord, vec2(.5, .5));
  float wavPos = sin(radians(360.0) * waves * 2.0 * (dist - speed * time));

  vec3 col = vec3(0.25, 0.25, 0.75);
  col += ((wavPos + 1.0) / 2.0) * 0.1;
  color = vec4(col, 1.0);
}
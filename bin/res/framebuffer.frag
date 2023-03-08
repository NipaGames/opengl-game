#version 330 core

out vec4 color;
in vec2 fragmentTexCoord;

uniform sampler2D screenTexture;

void main() { 
    color = vec4(vec3(1.0 - texture(screenTexture, fragmentTexCoord)), 1.0);
}
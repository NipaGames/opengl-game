#version 330 core

out vec4 color;
in vec2 fragmentTexCoord;

uniform sampler2D screenTexture;

void main() { 
    color = texture(screenTexture, fragmentTexCoord);
}
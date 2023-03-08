#version 330 core

out vec4 color;
in vec2 fragmentTexCoord;

uniform sampler2D screenTexture;

void main() {
    color = texture(screenTexture, fragmentTexCoord);
    float dist = distance(fragmentTexCoord, vec2(0.5, 0.5));
    color.rgb *= smoothstep(0.8, .5 * 0.799, dist * (.3 + .5));
}
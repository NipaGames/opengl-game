#version 330 core

out vec4 color;
in vec2 fragmentTexCoord;

uniform sampler2D screenTexture;

void main() {
    color = texture(screenTexture, fragmentTexCoord);
    float dist = distance(fragmentTexCoord, vec2(.5, .5));
    color.rgb *= smoothstep(.8, .5 * .8, dist * (.3 + .5));
}
#version 330 core

layout (location = 0) in vec3 pos;

out vec3 texCoords;

uniform mat4 projection;
uniform mat4 view;
uniform float clippingFar;

void main() {
    texCoords = pos;
    // add .001 to prevent z-fighting
    gl_Position = (projection * view * vec4(pos, .5 / clippingFar + 0.00001)).xyzw;
}
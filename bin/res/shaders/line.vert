#version 330 core
layout (location = 0) in vec3 pos0;
layout (location = 1) in vec3 pos1;

out VS_OUT {
    vec3 pos0;
    vec3 pos1;
} vs_out;

void main()
{
    vs_out.pos0 = pos0;
    vs_out.pos1 = pos1;
}
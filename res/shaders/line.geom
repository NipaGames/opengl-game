#version 330 core
layout (points) in;
layout (line_strip, max_vertices = 2) out;

in VS_OUT {
    vec3 pos0;
    vec3 pos1;
} gs_in[];

uniform mat4 projection;
uniform mat4 view;

void main() {
    gl_Position = projection * view * vec4(gs_in[0].pos0, 1.0); 
    EmitVertex();

    gl_Position = projection * view * vec4(gs_in[0].pos1, 1.0);
    EmitVertex();
    
    EndPrimitive();
}  
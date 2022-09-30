#include "graphics/material.h"

void Material::Use() {
    shader_.Use();
    for (auto i : intUniforms_)
        shader_.SetUniform(i.first, i.second);
    for (auto f : floatUniforms_)
        shader_.SetUniform(f.first, f.second);
    for (auto mat2 : mat2Uniforms_)
        shader_.SetUniform(mat2.first, mat2.second);
    for (auto mat3 : mat3Uniforms_)
        shader_.SetUniform(mat3.first, mat3.second);
    for (auto mat4 : mat4Uniforms_)
        shader_.SetUniform(mat4.first, mat4.second);
}
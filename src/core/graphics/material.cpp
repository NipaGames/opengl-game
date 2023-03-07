#include "core/graphics/material.h"

void Material::RestoreDefaultUniforms() {
    SetShaderUniform<glm::vec3>("color", glm::vec3(1.0f));
    SetShaderUniform<glm::vec3>("ambientColor", glm::vec3(0.0f));
}

void Material::Use() {
    shader_.Use();
    for (auto i : intUniforms_)
        shader_.SetUniform("material." + i.first, i.second);
    for (auto f : floatUniforms_)
        shader_.SetUniform("material." + f.first, f.second);
    for (auto mat2 : mat2Uniforms_)
        shader_.SetUniform("material." + mat2.first, mat2.second);
    for (auto mat3 : mat3Uniforms_)
        shader_.SetUniform("material." + mat3.first, mat3.second);
    for (auto mat4 : mat4Uniforms_)
        shader_.SetUniform("material." + mat4.first, mat4.second);
    for (auto vec2 : vec2Uniforms_)
        shader_.SetUniform("material." + vec2.first, vec2.second);
    for (auto vec3 : vec3Uniforms_)
        shader_.SetUniform("material." + vec3.first, vec3.second);
}
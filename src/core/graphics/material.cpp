#include "material.h"

void Material::RestoreDefaultUniforms() {
    SetShaderUniform<glm::vec3>("color", glm::vec3(1.0f));
    SetShaderUniform<glm::vec3>("ambientColor", glm::vec3(0.0f));
    SetShaderUniform<glm::vec2>("tiling", glm::vec2(1.0f));
    SetShaderUniform<glm::vec2>("offset", glm::vec2(0.0f));
}

void Material::Use(const Shader& shader) {
    if (texture_ == TEXTURE_NONE)
        shader.SetUniform("material.hasTexture", false);
    else
        shader.SetUniform("material.hasTexture", true);
    
    for (const auto& i : intUniforms_)
        shader.SetUniform(("material." + i.first).c_str(), i.second);
    for (const auto& f : floatUniforms_)
        shader.SetUniform(("material." + f.first).c_str(), f.second);
    for (const auto& mat2 : mat2Uniforms_)
        shader.SetUniform(("material." + mat2.first).c_str(), mat2.second);
    for (const auto& mat3 : mat3Uniforms_)
        shader.SetUniform(("material." + mat3.first).c_str(), mat3.second);
    for (const auto& mat4 : mat4Uniforms_)
        shader.SetUniform(("material." + mat4.first).c_str(), mat4.second);
    for (const auto& vec2 : vec2Uniforms_)
        shader.SetUniform(("material." + vec2.first).c_str(), vec2.second);
    for (const auto& vec3 : vec3Uniforms_)
        shader.SetUniform(("material." + vec3.first).c_str(), vec3.second);
    for (const auto& vec4 : vec4Uniforms_)
        shader.SetUniform(("material." + vec4.first).c_str(), vec4.second);
}

void Material::Use() {
    Use(shader_);
}
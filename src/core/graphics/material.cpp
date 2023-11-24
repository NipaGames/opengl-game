#include "material.h"

void Material::RestoreDefaultUniforms() {
    SetShaderUniform<glm::vec3>("color", glm::vec3(1.0f));
    SetShaderUniform<glm::vec3>("tint", glm::vec3(0.0f));
    SetShaderUniform<glm::vec3>("ambientColor", glm::vec3(0.0f));
    SetShaderUniform<glm::vec2>("tiling", glm::vec2(1.0f));
    SetShaderUniform<glm::vec2>("offset", glm::vec2(0.0f));
}

void Material::ClearUniforms() {
    intUniforms_.clear();
    floatUniforms_.clear();
    mat2Uniforms_.clear();
    mat3Uniforms_.clear();
    mat4Uniforms_.clear();
    vec2Uniforms_.clear();
    vec3Uniforms_.clear();
    vec4Uniforms_.clear();
}

void Material::Use(const Shader& shader) const {
    shader.SetUniform("material.hasTexture", texture_ != TEXTURE_NONE);
    
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

    if (cullFaces)
        glEnable(GL_CULL_FACE);
    else
        glDisable(GL_CULL_FACE);
}

void Material::Use() const {
    Use(shader_);
}
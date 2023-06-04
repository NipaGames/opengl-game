#include "material.h"

void Material::RestoreDefaultUniforms() {
    SetShaderUniform<glm::vec3>("color", glm::vec3(1.0f));
    SetShaderUniform<glm::vec3>("ambientColor", glm::vec3(0.0f));
}

void Material::Use() {
    if (texture_ == TEXTURE_NONE)
        shader_.SetUniform("material.hasTexture", false);
    else
        shader_.SetUniform("material.hasTexture", true);
    
    for (const auto& i : intUniforms_)
        shader_.SetUniform(("material." + i.first).c_str(), i.second);
    for (const auto& f : floatUniforms_)
        shader_.SetUniform(("material." + f.first).c_str(), f.second);
    for (const auto& mat2 : mat2Uniforms_)
        shader_.SetUniform(("material." + mat2.first).c_str(), mat2.second);
    for (const auto& mat3 : mat3Uniforms_)
        shader_.SetUniform(("material." + mat3.first).c_str(), mat3.second);
    for (const auto& mat4 : mat4Uniforms_)
        shader_.SetUniform(("material." + mat4.first).c_str(), mat4.second);
    for (const auto& vec2 : vec2Uniforms_)
        shader_.SetUniform(("material." + vec2.first).c_str(), vec2.second);
    for (const auto& vec3 : vec3Uniforms_)
        shader_.SetUniform(("material." + vec3.first).c_str(), vec3.second);
    for (const auto& vec4 : vec4Uniforms_)
        shader_.SetUniform(("material." + vec4.first).c_str(), vec4.second);
}
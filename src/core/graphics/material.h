#pragma once

#include <opengl.h>
#include <iostream>
#include <unordered_map>

#include "core/graphics/shader.h"
#include "core/graphics/texture.h"

class Material {
private:
    std::unordered_map<std::string, int> intUniforms_;
    std::unordered_map<std::string, float> floatUniforms_;

    std::unordered_map<std::string, glm::mat2> mat2Uniforms_;
    std::unordered_map<std::string, glm::mat3> mat3Uniforms_;
    std::unordered_map<std::string, glm::mat4> mat4Uniforms_;

    std::unordered_map<std::string, glm::vec2> vec2Uniforms_;
    std::unordered_map<std::string, glm::vec3> vec3Uniforms_;

    Shader shader_;
    Texture::TextureID texture_ = -1;
public:
    Material() { }
    Material(int shaderId) : shader_(shaderId) { }
    Material(int shaderId, Texture::TextureID textureId) : shader_(shaderId), texture_(textureId) { }
    void Use();
    const Shader& GetShader() { return shader_; }
    Texture::TextureID GetTexture() { return texture_; }

    template<typename T>
    void SetShaderUniform(const std::string& name, const T& value) {
        if constexpr(std::is_same_v<T, int>)
            intUniforms_[name] = value;
        if constexpr(std::is_same_v<T, float>)
            floatUniforms_[name] = value;
        if constexpr(std::is_same_v<T, glm::mat2>)
            mat2Uniforms_[name] = value;
        if constexpr(std::is_same_v<T, glm::mat3>)
            mat3Uniforms_[name] = value;
        if constexpr(std::is_same_v<T, glm::mat4>)
            mat4Uniforms_[name] = value;
        if constexpr(std::is_same_v<T, glm::vec2>)
            vec2Uniforms_[name] = value;
        if constexpr(std::is_same_v<T, glm::vec3>)
            vec3Uniforms_[name] = value;
    }
    template<typename T>
    T GetShaderUniform(const std::string& name) {
        if constexpr(std::is_same_v<T, int>)
            return intUniforms_[name];
        else if constexpr(std::is_same_v<T, float>)
            return floatUniforms_[name];
        else if constexpr(std::is_same_v<T, glm::mat2>)
            return mat2Uniforms_[name];
        else if constexpr(std::is_same_v<T, glm::mat3>)
            return mat3Uniforms_[name];
        else if constexpr(std::is_same_v<T, glm::mat4>)
            return mat4Uniforms_[name];
        else if constexpr(std::is_same_v<T, glm::vec2>)
            return vec2Uniforms_[name];
        else if constexpr(std::is_same_v<T, glm::vec3>)
            return vec3Uniforms_[name];
        else
            return T();
    }
};
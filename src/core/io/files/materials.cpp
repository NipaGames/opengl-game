#include "materials.h"
#include <core/game.h>

using namespace Serializer;
using json = nlohmann::json;

struct ShaderUniform {
    std::string name;
    GLenum type;
};

std::vector<ShaderUniform> ListMaterialUniforms(Shaders::ShaderID s) {
    GLint size; // size of the variable
    GLenum type; // type of the variable (float, vec3 or mat4, etc)

    const GLsizei bufSize = 32; // maximum name length
    GLchar name[bufSize]; // variable name in GLSL
    GLsizei length; // name length

    GLint count;
    glGetProgramiv(Shaders::GetShaderProgram(s), GL_ACTIVE_UNIFORMS, &count);

    std::vector<ShaderUniform> uniforms;
    for (int i = 0; i < count; i++) {
        glGetActiveUniform(Shaders::GetShaderProgram(s), (GLuint)i, bufSize, &length, &size, &type, name);
        std::string nameStr = name;
        std::string prefix = "material.";
        size_t prefixPos = nameStr.rfind(prefix, 0);
        if (prefixPos != 0)
            continue;
        uniforms.push_back({ nameStr.substr(prefix.length()), type });
    }
    return uniforms;
}

std::pair<std::string, std::shared_ptr<Material>> ParsingException(int* invalidMaterials = nullptr) {
    if (invalidMaterials != nullptr)
        (*invalidMaterials)++;
    return { "", nullptr };
}

std::pair<std::string, std::shared_ptr<Material>> ParseMaterial(const json& materialJson, int* invalidMaterials = nullptr) {
    if (!materialJson.is_object())
        return ParsingException(invalidMaterials);
    
    std::shared_ptr<Material> m = std::make_shared<Material>();
    if (!materialJson.contains("id") || !materialJson["id"].is_string())
        return ParsingException(invalidMaterials);
    
    std::string id = materialJson["id"];
    if (!materialJson.contains("shader"))
        return ParsingException(invalidMaterials);
    
    Shaders::ShaderID shader;
    if(!SetJSONPointerValue(&shader, materialJson["shader"]))
        return ParsingException(invalidMaterials);
    
    m->SetShader(shader);

    if (materialJson.contains("texture")) {
        const json& textureJson = materialJson["texture"];
        if (!textureJson.is_object() || !textureJson.contains("path") || !textureJson["path"].is_string())
            return ParsingException(invalidMaterials);
        m->SetTexture(Texture::GetTexture(textureJson["path"]));
    }
    
    if (materialJson.contains("uniforms")) {
        const json& uniformsJson = materialJson["uniforms"];
        if (!uniformsJson.is_object())
            return ParsingException(invalidMaterials);
        auto uniforms = ListMaterialUniforms(shader);
        for (const auto& u : uniformsJson.items()) {
            auto it = std::find_if(uniforms.begin(), uniforms.end(), [&](const ShaderUniform& uniform) { return u.key() == uniform.name; });
            if (it == uniforms.end())
                continue;
            ShaderUniform uniform = *it;
            switch (uniform.type) {
                case GL_FLOAT:
                    float f;
                    if(!SetJSONPointerValue(&f, u.value()))
                        break;
                    m->SetShaderUniform(uniform.name, f);
                    break;
                case GL_INT:
                    int i;
                    if(!SetJSONPointerValue(&i, u.value()))
                        break;
                    m->SetShaderUniform(uniform.name, i);
                    break;
                case GL_INT_VEC2:
                case GL_FLOAT_VEC2:
                    glm::vec2 vec2;
                    if(!SetJSONPointerValue(&vec2, u.value()))
                        break;
                    m->SetShaderUniform(uniform.name, vec2);
                    break;
                case GL_INT_VEC3:
                case GL_FLOAT_VEC3:
                    glm::vec3 vec3;
                    if(!SetJSONPointerValue(&vec3, u.value()))
                        break;
                    m->SetShaderUniform(uniform.name, vec3);
                    break;
            }
        }
    }
    return { id, m };
}

std::unordered_map<std::string, std::shared_ptr<Material>>& MaterialSerializer::GetMaterials() {
    return materials_;
}

void MaterialSerializer::AddMaterials() {
    for (auto& m : materials_) {
        game->GetRenderer().materials.insert(m);
    }
}

void MaterialSerializer::ParseJSON() {
    if (!jsonData_.is_array()) {
        spdlog::error("[" + path_ + "] Must be an array!");
        return;
    }
    int invalidMaterials = 0;
    for (const auto& m : jsonData_.items()) {
        auto parsed = ParseMaterial(m.value(), &invalidMaterials);
        if (parsed.second != nullptr)
            materials_.insert(parsed);
    }
    if (invalidMaterials > 0) {
        spdlog::warn("[" + path_ + "] " + std::to_string(invalidMaterials) + " invalid materials!");
    }
}
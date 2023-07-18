#include "serializetypes.h"
#include "serializer.h"

#include <core/graphics/shader.h>
#include <core/graphics/component/meshrenderer.h>

template<typename T>
bool SerializeJSONNumber(Serializer::SerializationArgs& args, const nlohmann::json& j) {
    if (!j.is_number())
        return false;
    args.Return<T>(j);
    return true;
}

template<size_t S>
bool SerializeJSONVector(Serializer::SerializationArgs& args, const nlohmann::json& j) {
    glm::vec<S, float> vec;
    if (j.is_number()) vec = glm::vec3(j);
    else if (j.is_array() && j.size() == S) {
        for (int i = 0; i < S; i++) {
            if (!j[i].is_number()) return false;
            vec[i] = j[i];
        }
    }
    else return false;
    args.Return(vec);
    return true;
}

void RegisterDefaultSerializers() {
    // JSON serializers
    Serializer::AddJSONSerializer<int>(SerializeJSONNumber<int>);
    Serializer::AddJSONSerializer<float>(SerializeJSONNumber<float>);
    Serializer::AddJSONSerializer<glm::vec2, glm::ivec2>(SerializeJSONVector<2>);
    Serializer::AddJSONSerializer<glm::vec3, glm::ivec3>(SerializeJSONVector<3>);
    Serializer::AddJSONSerializer<bool>([](Serializer::SerializationArgs& args, const nlohmann::json& j) {
        if (!j.is_boolean())
            return false;
        args.Return((bool) j);
        return true;
    });
    Serializer::AddJSONSerializer<Shaders::ShaderID>([](Serializer::SerializationArgs& args, const nlohmann::json& j) {
        if (!j.is_string())
            return false;
        auto s = magic_enum::enum_cast<Shaders::ShaderID>((std::string) j);
        if (!s.has_value())
            return false;
        args.Return(s.value());
        return true;
    });
    // just a placeholder for now
    Serializer::AddJSONSerializer<std::shared_ptr<Mesh>>([](Serializer::SerializationArgs& args, const nlohmann::json& j) {
        auto mesh = Meshes::CreateMeshInstance(Meshes::CUBE);
        mesh->material = std::make_shared<Material>(Shaders::ShaderID::LIT);
        mesh->material->SetShaderUniform<int>("specularHighlight", 8);
        mesh->material->SetShaderUniform<float>("specularStrength", 0);
        args.Return(mesh);
        return true;
    });
}

#ifndef JSON_DEFAULT_SERIALIZATIONS
// global scope macro serializer registrations here
#define JSON_DEFAULT_SERIALIZATIONS
#endif
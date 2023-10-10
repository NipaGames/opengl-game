#include "serializetypes.h"
#include "serializer.h"

#include <core/game.h>
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
    Serializer::AddJSONSerializer<glm::quat>([](Serializer::SerializationArgs& args, const nlohmann::json& j) {
        glm::vec3 eulers;
        Serializer::SerializationArgs vecArgs(Serializer::SerializerType::ANY_POINTER);
        vecArgs.ptr = &eulers;
        if (!SerializeJSONVector<3>(vecArgs, j))
            return false;
        args.Return(glm::quat(glm::radians(eulers)));
        return false;
    });
    Serializer::AddJSONSerializer<std::string>([](Serializer::SerializationArgs& args, const nlohmann::json& j) {
        if (j.is_string()) {
            args.Return((std::string) j);
            return true;
        }
        return false;
    });
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
        mesh->material = game->GetRenderer().GetMaterial("MAT_DEFAULT");
        args.Return(mesh);
        return true;
    });
}

#ifndef JSON_DEFAULT_SERIALIZATIONS
// global scope macro serializer registrations here
#define JSON_DEFAULT_SERIALIZATIONS
#endif
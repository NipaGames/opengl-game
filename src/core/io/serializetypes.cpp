#include "serializetypes.h"
#include "serializer.h"

#include <core/graphics/shader.h>
#include <core/graphics/component/meshrenderer.h>

#ifndef JSON_DEFAULT_SERIALIZATIONS

JSON_SERIALIZE_TYPES([](Serializer::SerializationArgs& args, const nlohmann::json& j) {
    if (!j.is_number())
        return false;
    args.Return<int>(j);
    return true;
}, int);

JSON_SERIALIZE_TYPES([](Serializer::SerializationArgs& args, const nlohmann::json& j) {
    if (!j.is_number())
        return false;
    args.Return<float>(j);
    return true;
}, float);

JSON_SERIALIZE_TYPES([](Serializer::SerializationArgs& args, const nlohmann::json& j) {
    glm::vec3 vec;
    if (j.is_number()) vec = glm::vec3(j);
    else if (j.is_array() && j.size() == 3) {
        for (int i = 0; i < 3; i++) {
            if (!j[i].is_number()) return false;
            vec[i] = j[i];
        }
    }
    else return false;
    args.Return(vec);
    return true;
}, glm::vec3, glm::ivec3);

JSON_SERIALIZE_TYPES([](Serializer::SerializationArgs& args, const nlohmann::json& j) {
    glm::vec2 vec;
    if (j.is_number()) vec = glm::vec3(j);
    else if (j.is_array() && j.size() == 2) {
        for (int i = 0; i < 2; i++) {
            if (!j[i].is_number()) return false;
            vec[i] = j[i];
        }
    }
    else return false;
    args.Return(vec);
    return true;
}, glm::vec2, glm::ivec2);

JSON_SERIALIZE_TYPES([](Serializer::SerializationArgs& args, const nlohmann::json& j) {
    if (!j.is_string())
        return false;
    auto s = magic_enum::enum_cast<Shaders::ShaderID>((std::string) j);
    if (!s.has_value())
        return false;
    args.Return(s.value());
    return true;
}, Shaders::ShaderID);

// just a placeholder for now
JSON_SERIALIZE_TYPES([](Serializer::SerializationArgs& args, const nlohmann::json& j) {
    auto mesh = Meshes::CreateMeshInstance(Meshes::CUBE);
    mesh->material = std::make_shared<Material>(Shaders::ShaderID::LIT);
    mesh->material->SetShaderUniform<int>("specularHighlight", 8);
    mesh->material->SetShaderUniform<float>("specularStrength", 0);
    args.Return(mesh);
    return true;
}, std::shared_ptr<Mesh>);

#define JSON_DEFAULT_SERIALIZATIONS
#endif
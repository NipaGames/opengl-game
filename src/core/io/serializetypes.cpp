#include "serializetypes.h"
#include "serializer.h"
#include "files/cfg.h"

#include <core/game.h>
#include <core/graphics/shader.h>
#include <core/graphics/component/meshrenderer.h>

template <typename T>
bool SerializeJSONNumber(Serializer::SerializationArgs& args, const nlohmann::json& j) {
    if (!j.is_number())
        return false;
    args.Return<T>(j);
    return true;
}

template <size_t S>
bool SerializeJSONVector(Serializer::SerializationArgs& args, const nlohmann::json& j) {
    typedef glm::vec<S, float> Vector;
    Vector vec;
    if (j.is_number()) vec = Vector(j);
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

template <size_t S, typename T>
bool SerializeCFGVector(Serializer::SerializationArgs& args, const CFG::ICFGField* field, CFG::CFGFieldType type) {
    if (field->type != CFG::CFGFieldType::STRUCT && field->type != CFG::CFGFieldType::ARRAY)
        return false;
    const CFG::CFGObject* obj = static_cast<const CFG::CFGObject*>(field);
    const auto& items = obj->GetItems();
    if (items.size() != S)
        return false;
    glm::vec<S, T> vec;
    for (int i = 0; i < S; i++) {
        const CFG::ICFGField* child = items.at(i);
        if (!IsValidType(child->type, type))
            return false;
        vec[i] = child->GetValue<T>();
    }
    args.Return(vec);
    return true;
}

template <size_t S, typename T, CFG::CFGFieldType F>
bool SerializeCFGVector(Serializer::SerializationArgs& args, const CFG::ICFGField* field) {
    return SerializeCFGVector<S, T>(args, field, F);
}

template <size_t S, typename T>
bool SerializeCFGVectorAuto(Serializer::SerializationArgs& args, const CFG::ICFGField* field) {
    const type_info* type = &typeid(T);
    if (CFG::CFG_TYPES.count(type) == 0)
        return false;
    return SerializeCFGVector<S, T>(args, field, CFG::CFG_TYPES.at(type).at(0));
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
        return true;
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

    // CFG serializers
    using namespace CFG;
    Serializer::AddCFGSerializer<float>([](Serializer::SerializationArgs& args, const ICFGField* field) {
        if (!IsValidType(field->type, CFGFieldType::FLOAT))
            return false;
        args.Return(field->GetValue<float>());
        return true;
    });
    Serializer::AddCFGSerializer<int, bool>([](Serializer::SerializationArgs& args, const ICFGField* field) {
        if (!IsValidType(field->type, CFGFieldType::INTEGER))
            return false;
        args.Return(field->GetValue<int>());
        return true;
    });

    Serializer::AddCFGSerializer<glm::vec2>(SerializeCFGVectorAuto<2, float>);
    Serializer::AddCFGSerializer<glm::ivec2>(SerializeCFGVectorAuto<2, int>);
}

#ifndef JSON_DEFAULT_SERIALIZATIONS
// global scope macro serializer registrations here
#define JSON_DEFAULT_SERIALIZATIONS
#endif
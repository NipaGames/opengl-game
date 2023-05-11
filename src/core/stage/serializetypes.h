#pragma once

#define _UNIQUE_VAR_NAME_CONCAT_(x, y) x##y
#define _UNIQUE_VAR_NAME_CONCAT(x, y) _UNIQUE_VAR_NAME_CONCAT_(x, y)
#define _UNIQUE_VAR_NAME(x) _UNIQUE_VAR_NAME_CONCAT(x, __COUNTER__)
#define STAGE_SERIALIZE_TYPES(f, ...) inline const void* _UNIQUE_VAR_NAME(_stage_serialization_init_val_) = Stage::AddSerializer<__VA_ARGS__>(f)

#ifndef STAGE_DEFAULT_SERIALIZATIONS
#include "core/stage/stage.h"
STAGE_SERIALIZE_TYPES([](ComponentData& data, const std::string& k, const nlohmann::json& j) {
    if (!j.is_number())
        return false;
    data.Set(k, (int) j);
    return true;
}, int);

STAGE_SERIALIZE_TYPES([](ComponentData& data, const std::string& k, const nlohmann::json& j) {
    if (!j.is_number())
        return false;
    data.Set(k, (float) j);
    return true;
}, float);

STAGE_SERIALIZE_TYPES([](ComponentData& data, const std::string& k, const nlohmann::json& j) {
    glm::vec3 vec;
    if (j.is_number()) vec = glm::vec3(j);
    else if (j.is_array() && j.size() == 3) {
        for (int i = 0; i < 3; i++) {
            if (!j[i].is_number()) return false;
            vec[i] = j[i];
        }
    }
    else return false;
    data.Set(k, vec);
    return true;
}, glm::vec3, glm::ivec3);

STAGE_SERIALIZE_TYPES([](ComponentData& data, const std::string& k, const nlohmann::json& j) {
    glm::vec2 vec;
    if (j.is_number()) vec = glm::vec3(j);
    else if (j.is_array() && j.size() == 2) {
        for (int i = 0; i < 2; i++) {
            if (!j[i].is_number()) return false;
            vec[i] = j[i];
        }
    }
    else return false;
    data.Set(k, vec);
    return true;
}, glm::vec2, glm::ivec2);
#define STAGE_DEFAULT_SERIALIZATIONS
#endif
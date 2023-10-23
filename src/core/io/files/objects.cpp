#include "objects.h"

#include <core/game.h>

using namespace Serializer;
using namespace nlohmann;

typedef std::pair<std::string, Object> ObjectPair;

std::optional<ObjectPair> ParseObject(const json& objJson) {
    if (!objJson.is_object())
        return std::nullopt;
    if (!objJson.contains("model"))
        return std::nullopt;
    if (!objJson.at("model").is_string())
        return std::nullopt;
    std::string modelId = objJson.at("model");
    std::string defaultMaterialId;
    Object obj;
    if (objJson.contains("defaultMaterial")) {
        if (!objJson.at("defaultMaterial").is_string())
            return std::nullopt;
        defaultMaterialId = objJson.at("defaultMaterial");
        obj.defaultMaterial = GAME->GetRenderer().GetMaterial(defaultMaterialId);
    }
    if (objJson.contains("size")) {
        if (!SetJSONPointerValue(&obj.size, objJson.at("size")))
            return std::nullopt;
    }
    if (objJson.contains("materialOverrides")) {
        if (!objJson.at("materialOverrides").is_object())
            return std::nullopt;
        for (const auto& mat : objJson.at("materialOverrides").items()) {
            std::string index = mat.key();
            if (index.empty())
                return std::nullopt;
            // check for a valid integer value
            if (std::find_if(index.begin(), index.end(), [](unsigned char c) { return !std::isdigit(c); }) != index.end())
                return std::nullopt;
            int i = std::stoi(index);
            if (!mat.value().is_string())
                return std::nullopt;
            std::string matId = mat.value();
            if (matId.at(0) == ':' && !defaultMaterialId.empty())
                matId = defaultMaterialId + matId;
            obj.materials.insert({ i, GAME->GetRenderer().GetMaterial(matId) });   
        }
    }
    return std::make_optional<ObjectPair>(modelId, obj);
}

void ObjectSerializer::ParseJSON() {
    if (!jsonData_.is_array()) {
        spdlog::error("[" + path_ + "] Must be an array!");
        return;
    }
    for (const auto& obj : jsonData_.items()) {
        auto parsed = ParseObject(obj.value());
        if (parsed.has_value())
            items_.insert(parsed.value());
    }
}
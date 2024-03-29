#include "serializer.h"

using namespace Serializer;

bool Serializer::SetJSONComponentValue(IComponent* c, const std::string& k, const nlohmann::json& jsonVal, const std::string& entityId) {
    auto dataVal = c->data.GetComponentDataValue(k);
    if (dataVal == nullptr)
        return false;
    
    auto it = std::find_if(Serializer::JSON_SERIALIZERS.begin(), Serializer::JSON_SERIALIZERS.end(), [&](const auto& s) {
        return s->CompareToComponentType(c->data.GetComponentDataValue(k));
    });
    if (it == Serializer::JSON_SERIALIZERS.end())
        return false;
    
    const auto& serializer = *it;
    SerializationArgs args(SerializerType::COMPONENT_DATA);
    args.entityId = entityId;
    switch (dataVal->componentType) {
        case ComponentDataValueType::SINGLE:
            args.ctData = &c->data;
            args.ctK = k;
            return (serializer->fn)(args, jsonVal);
        case ComponentDataValueType::VECTOR:
            if (!jsonVal.is_array())
                return false;

            ComponentData arrayWrapper;
            args.ctData = &arrayWrapper;
            bool hasAnyFailed = false;
            int i = 0;
            for (auto e : jsonVal) {
                std::string key = std::to_string(i++);
                args.ctK = key;
                if (!(serializer->fn)(args, e))
                    hasAnyFailed = true;
            }
            dataVal->CopyValuesFromComponentDataArray(arrayWrapper);
            return !hasAnyFailed;
    }
    return false; 
}

void IFileSerializer::SerializeFile() {
    std::ifstream f(path_);
    if (f.fail()) {
        spdlog::error("Cannot read file '" + path_ + "'!");
        status_ = SerializationStatus::FAILED;
        return;
    }
    status_ = ParseContents(f) ? SerializationStatus::OK : SerializationStatus::FAILED;
}

void IFileSerializer::SerializeFile(const std::string& p) {
    path_ = p;
    SerializeFile();
}

bool JSONFileSerializer::ParseContents(std::ifstream& f) {
    try {
        jsonData_ = nlohmann::json::parse(f);
    }
    catch (std::exception& e) {
        spdlog::error("[" + path_ + "] Invalid JSON syntax!");
        spdlog::debug(e.what());
        return false;
    }
    return ParseJSON();
}
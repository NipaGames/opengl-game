#include "serializer.h"

using namespace Serializer;

bool Serializer::SetJSONComponentValue(IComponent* c, const std::string& k, const nlohmann::json& jsonVal) {
    auto dataVal = c->data.GetComponentDataValue(k);
    if (dataVal == nullptr)
        return false;
    
    auto it = std::find_if(Serializer::JSON_COMPONENT_VAL_SERIALIZERS.begin(), Serializer::JSON_COMPONENT_VAL_SERIALIZERS.end(), [&](const auto& s) {
        return s->CompareType(c->data.GetComponentDataValue(k));
    });
    if (it == Serializer::JSON_COMPONENT_VAL_SERIALIZERS.end())
        return false;
    
    const auto& serializer = *it;
    SerializationArgs args(SerializerType::COMPONENT_DATA);
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
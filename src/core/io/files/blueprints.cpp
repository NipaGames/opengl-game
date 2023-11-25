#include "blueprints.h"
#include "stage.h"

using namespace Serializer;

BlueprintSerializer::~BlueprintSerializer() {
    for (const auto& [id, components] : items_) {
        for (IComponent* c : components) {
            delete c;
        }
    }
    items_.clear();
}

bool BlueprintSerializer::ParseJSON() {
    if (!jsonData_.is_object()) {
        spdlog::error("Blueprints must be an object!");
        return false;
    }
    for (const auto& [id, componentsJson] : jsonData_.items()) {
        std::vector<IComponent*> components;
        if (!componentsJson.is_object()) {
            spdlog::error("A blueprint must be an object!");
            continue;
        }
        for (const auto& [ck, cv] : componentsJson.items()) {
            if (!cv.is_object())
                continue;
            IComponent* c = IComponent::CreateComponent(ck);    
            bool success = DeserializeComponentFromJSON(c, cv);
            if (!success) {
                spdlog::warn("Failed deserializing component '{}' for a blueprint!", ck);
            }
            components.push_back(c);
        }
        items_.insert({ id, components });
    }
    return true;
}
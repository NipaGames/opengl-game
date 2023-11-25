#include "stage.h"

using namespace Serializer;
using json = nlohmann::json;

bool Serializer::DeserializeComponentFromJSON(IComponent* c, const nlohmann::json& json, const std::string& entityId) {
    if (c == nullptr) {
        return false;
    }
    std::vector<std::shared_ptr<IComponentDataValue>> affectedValPtrs;
    for (const auto&[k, v] : json.items()) {
        if (Serializer::SetJSONComponentValue(c, k, v, entityId)) {
            affectedValPtrs.push_back(c->data.vars.at(k));
        }
        else {
            return false;
        }
    }
    for (auto it = c->data.vars.cbegin(); it != c->data.vars.cend();) {
        if (std::find(affectedValPtrs.begin(), affectedValPtrs.end(), it->second) == affectedValPtrs.end())
            c->data.vars.erase(it++);
        else
            ++it;
    }
    return true;
}

std::list<Entity> StageSerializer::ParseEntities(const json& entities, int* invalidEntities) {
    std::list<Entity> parsedEntities;
    for (const auto&[ek, ev] : entities.items()) {
        if (ev.is_object()) {
            Entity entity("");
            entity.RemoveComponent<Transform>();
            entity.transform = nullptr;
            if (ev.contains("id")) {
                if (ev["id"].is_string())
                    entity.id = ev["id"];
            }
            std::string blueprint;
            if (ev.contains("blueprint")) {
                if (ev["blueprint"].is_string())
                    blueprint = ev["blueprint"];
            }
            if (!blueprint.empty() && blueprints_ != nullptr) {
                if (blueprints_->HasItem(blueprint)) {
                    const std::vector<IComponent*>& blueprintComponents = blueprints_->GetItem(blueprint);
                    for (const IComponent* c : blueprintComponents) {
                        IComponent* entityComponent = entity.GetComponent(c->typeHash);
                        if (entityComponent == nullptr)
                            entityComponent = entity.AddComponent(c->typeHash);
                        for (const auto& [varName, varValue] : c->data.vars) {
                            varValue->CloneValuesTo(entityComponent->data.GetComponentDataValue(varName));
                        }
                    }
                }
                else {
                    spdlog::warn("Cannot find blueprint '{}'!", blueprint);
                }
            }
            for (const auto&[ck, cv] : ev.items()) {
                if (!cv.is_object())
                    continue;
                IComponent* c = entity.GetComponent(ck);
                
                if (c == nullptr)
                    c = entity.AddComponent(ck);
                
                bool success = DeserializeComponentFromJSON(c, cv, entity.id);
                if (!success) {
                    spdlog::warn("Failed deserializing component '{}'!", ck);
                    // yeah yeah the whole entity isn't necessarily invalid but this will do now
                    if (invalidEntities != nullptr)
                        (*invalidEntities)++;
                }
            }
            parsedEntities.push_back(entity);
        }
        else if (ev.is_array()) {
            std::list<Entity> recursiveEntities = ParseEntities(ev, invalidEntities);
            parsedEntities.insert(parsedEntities.end(), recursiveEntities.begin(), recursiveEntities.end());
        }
        else if (invalidEntities != nullptr) {
            (*invalidEntities)++;
        }
    }
    return parsedEntities;
}

bool StageSerializer::ParseJSON() {
    if (!jsonData_.contains("id") || !jsonData_["id"].is_string()) {
        spdlog::error("[" + path_ + "] Missing 'id'!");
        return false;
    }
    stage_.id = jsonData_["id"];
    if (jsonData_.contains("data") && jsonData_["data"].is_object())
        stage_.data = jsonData_["data"];
    int invalidEntities = 0;
    if (jsonData_.contains("entities")) {
        json& entities = jsonData_["entities"];
        if (entities.is_array()) {
            stage_.entities = ParseEntities(entities, &invalidEntities);
        }
        else
            spdlog::warn("[" + path_ + "] 'entities' must be an array!");
    }
    if (invalidEntities > 0) {
        spdlog::warn("[" + path_ + "] " + std::to_string(invalidEntities) + " invalid entities!");
    }
    return true;
}

Stage& StageSerializer::GetStage() {
    return stage_;
}
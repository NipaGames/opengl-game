#include "stage.h"

using namespace Serializer;
using json = nlohmann::json;

std::list<Entity> ParseEntities(const json& entities, int* invalidEntities = nullptr) {
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
            for (const auto&[ck, cv] : ev.items()) {
                if (!cv.is_object())
                    continue;
                std::vector<std::shared_ptr<IComponentDataValue>> affectedValPtrs;
                IComponent* c = entity.GetComponent(ck);
                if (c == nullptr)
                    c = entity.AddComponent(ck);
                
                if (c == nullptr) {
                    spdlog::warn("Cannot find component '" + ck + "'!");
                    // yeah yeah the whole entity isn't necessarily invalid but this will do now
                    (*invalidEntities)++;
                    continue;
                }
                for (const auto&[k, v] : cv.items()) {
                    if (Serializer::SetJSONComponentValue(c, k, v)) {
                        affectedValPtrs.push_back(c->data.vars.at(k));
                    }
                    else {
                        spdlog::warn("Invalid or empty value for component '" + ck + "'!");
                        (*invalidEntities)++;
                    }
                }
                for (auto it = c->data.vars.cbegin(); it != c->data.vars.cend();) {
                    if (std::find(affectedValPtrs.begin(), affectedValPtrs.end(), it->second) == affectedValPtrs.end())
                        c->data.vars.erase(it++);
                    else
                        ++it;
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

void StageSerializer::ParseJSON() {
    json& root = jsonData_.items().begin().value();
    if (!root.is_object()) {
        spdlog::error("[" + path_ + "] Root must be an object!");
        return;
    }
    stage_.id = jsonData_.items().begin().key();
    int invalidEntities = 0;
    if (root.contains("entities")) {
        json& entities = root["entities"];
        if (entities.is_array()) {
            stage_.entities = ParseEntities(entities, &invalidEntities);
        }
        else
            spdlog::warn("[" + path_ + "] 'entities' must be an array!");
    }
    if (invalidEntities > 0) {
        spdlog::warn("[" + path_ + "] '" + std::to_string(invalidEntities) + " invalid entities!");
    }
}

Stage::Stage& StageSerializer::GetStage() {
    return stage_;
}
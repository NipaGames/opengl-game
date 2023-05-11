#include "core/stage/stage.h"
#include "core/stage/serializetypes.h"

#include <fstream>
#include <unordered_map>
#include <spdlog/spdlog.h>

#include "core/game.h"
#include "core/entity/entitymanager.h"

using json = nlohmann::json;

std::vector<Stage::Stage> stages;
std::vector<std::string> loadedStages;

bool SetComponentValue(IComponent* c, const std::string& k, const json& jsonVal) {
    auto dataVal = c->data.GetComponentDataValue(k);
    if (dataVal == nullptr)
        return false;
    
    switch (dataVal->componentType) {
        case ComponentDataValueType::SINGLE:
            for (const auto& v : Stage::_COMPONENT_VAL_SERIALIZERS) {
                if (v->CompareType(c->data.GetComponentDataValue(k)))
                    return (v->fn)(c->data, k, jsonVal);
            }
            break;
        case ComponentDataValueType::VECTOR:
            for (const auto& v : Stage::_COMPONENT_VAL_SERIALIZERS) {
                if (v->CompareType(c->data.GetComponentDataValue(k))) {
                    if (!jsonVal.is_array())
                        return false;

                    ComponentData arrayWrapper;
                    bool hasAnyFailed = false;
                    int i = 0;
                    for (auto e : jsonVal) {
                        std::string key = std::to_string(i++);
                        if (!(v->fn)(arrayWrapper, key, e))
                            hasAnyFailed = true;
                    }
                    dataVal->CopyValuesFromComponentDataArray(arrayWrapper);
                    return !hasAnyFailed;
                }
            }
            break;
    }

    return false; 
}

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
                    if (SetComponentValue(c, k, v)) {
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

Stage::Stage Stage::ReadStageFromFile(const std::string& path) {
    Stage s;
    std::ifstream f(path);
    if (f.fail()) {
        spdlog::error("Stage '" + path + "' does not exist!");
        return s;
    }
    json data = json::parse(f);
    if (!data.is_object() || data.size() == 0) {
        spdlog::error("[" + path + "] Missing a root element!");
        return s;
    }
    json& root = data.items().begin().value();
    if (!root.is_object()) {
        spdlog::error("[" + path + "] Root must be an object!");
        return s;
    }
    s.id = data.items().begin().key();
    int invalidEntities = 0;
    if (root.contains("entities")) {
        json& entities = root["entities"];
        if (entities.is_array()) {
            s.entities = ParseEntities(entities, &invalidEntities);
        }
        else
            spdlog::warn("[" + path + "] 'entities' must be an array!");
    }
    if (invalidEntities > 0) {
        spdlog::warn("[" + path + "] '" + std::to_string(invalidEntities) + " invalid entities!");
    }

    return s;
}

void Stage::AddStage(const Stage& stage) {
    if (!stage.id.empty())
        stages.push_back(stage);
}

bool Stage::LoadStage(const std::string& id) {
    if (stages.empty())
        return false;
    auto sIt = std::find_if(stages.begin(), stages.end(), [&](const Stage& s) { return s.id == id; });
    if (sIt == stages.end())
        return false;
    Stage& s = *sIt;

    for (const Entity& e : s.entities) {
        if (e.id.empty()) {
            Entity& instantiated = game->GetEntityManager().AddEntity(e);
            s.instantiatedEntities.insert(instantiated.GetHash());
            instantiated.Start();
        }
        else {
            bool hasEntityAlready = game->GetEntityManager().CountEntities(e.id) > 0;
            Entity& entity = game->GetEntityManager()[e.id];
            entity.OverrideComponentValues(e);
            if (!hasEntityAlready) {
                s.instantiatedEntities.insert(entity.GetHash());
            }
            entity.Start();
        }
    }
    loadedStages.insert(loadedStages.begin(), s.id);
    spdlog::info("Loaded stage '" + id + "' (" + std::to_string(s.entities.size()) + " entities modified)");
    game->GetRenderer().UpdateLighting();
    return true;
}

bool Stage::UnloadStage(const std::string& id) {
    auto idIt = std::find(loadedStages.begin(), loadedStages.end(), id);
    if (idIt == loadedStages.end())
        return false;
    auto sIt = std::find_if(stages.begin(), stages.end(), [&](const Stage& s) { return s.id == id; });
    Stage& s = *sIt;
    for (size_t hash : s.instantiatedEntities) {
        game->GetEntityManager().RemoveEntity(hash);
    }
    loadedStages.erase(idIt);
    game->GetRenderer().UpdateLighting();
    return true;
}

void Stage::UnloadAllStages() {
    for (const std::string& s : loadedStages) {
        UnloadStage(s);
    }
}

const std::vector<std::string>& Stage::GetLoadedStages() {
    return loadedStages;
}
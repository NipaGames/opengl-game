#include "core/stage/stage.h"

#include <fstream>
#include <unordered_map>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

#include "core/game.h"
#include "core/entity/entitymanager.h"

using json = nlohmann::json;

std::vector<Stage::Stage> stages;
std::vector<std::string> loadedStages;

#define STAGE_JSON_ENTITIES_KEY "entities"
#define STAGE_JSON_ENTITIES_AUTO_KEY "entities-auto"

// this is just messed up, sorry
// may god have mercy on anyone who has chosen to read the following code
// -----------------------------
typedef std::function<bool(IComponent*, const std::string&, const json&)> ValueInitializer;
typedef std::vector<std::pair<std::vector<size_t>, ValueInitializer>> InitializerTable;
InitializerTable componentValInitializers;
// must return something
template<typename... C>
void* AddInitializer(const ValueInitializer& init) {
    std::vector<size_t> hashes;
    ([&] {
       hashes.push_back(typeid(C).hash_code());
    } (), ...);
    componentValInitializers.push_back({ hashes, init });
    return nullptr;
}

// yeah, it would be a great idea to use the line number to create unique variable names
#define INIT_TYPES_LN_(ln, f, ...) void* _init_ln##ln = AddInitializer<__VA_ARGS__>(f)
#define INIT_TYPES_LN(ln, f, ...) INIT_TYPES_LN_(ln, f, __VA_ARGS__)
#define INIT_TYPES(f, ...) INIT_TYPES_LN(__LINE__, f, __VA_ARGS__)

INIT_TYPES([](IComponent* c, const std::string& k, const json& j) {
    if (!j.is_number())
        return false;
    if (c->data.vars[k]->type->hash_code() == typeid(double).hash_code())
        c->data.Set(k, (double) j);
    else if (c->data.vars[k]->type->hash_code() == typeid(float).hash_code())
        c->data.Set(k, (float) j);
    else if (c->data.vars[k]->type->hash_code() == typeid(int).hash_code())
        c->data.Set(k, (int) j);
    return true;
}, double, float, int);

INIT_TYPES([](IComponent* c, const std::string& k, const json& j) {
    glm::vec3 vec;
    if (j.is_number()) vec = glm::vec3(j);
    else if (j.is_array() && j.size() == 3) {
        for (int i = 0; i < 3; i++) {
            if (!j[i].is_number()) return false;
            vec[i] = j[i];
        }
    }
    else return false;
    c->data.Set(k, vec);
    return true;
}, glm::vec3, glm::ivec3);
// --------------------------
// normal code continues here

bool SetComponentValue(IComponent* c, const std::string& k, const json& jsonVal) {
    const type_info* type = c->data.GetType(k);
    if (type == nullptr)
        return false;
    
    const ValueInitializer* init = nullptr;
    for (const auto& pair : componentValInitializers) {
        auto it = std::find(pair.first.begin(), pair.first.end(), type->hash_code());
        if (it != pair.first.end()) {
            init = &pair.second;
            break;
        }        
    }
    if (init != nullptr)
        return (*init)(c, k, jsonVal);
    return false; 
}

std::vector<Entity> ParseEntities(const json& entities, bool autoEnabled, int* invalidEntities = nullptr) {
    std::vector<Entity> parsedEntities;
    for (const auto&[ek, ev] : entities.items()) {
        if (ev.is_object()) {
            Entity entity("");
            if (!autoEnabled)
                entity.id = ek;
            for (const auto&[ck, cv] : ev.items()) {
                if (!cv.is_object())
                    continue;
                IComponent* c = entity.GetComponent(ck);
                if (c == nullptr)
                    c = entity.AddComponent(ck);
                
                for (const auto&[k, v] : cv.items()) {
                    SetComponentValue(c, k, v);
                }
            }
            parsedEntities.push_back(entity);
        }
        else if (ev.is_array()) {
            std::vector<Entity> recursiveEntities = ParseEntities(ev, autoEnabled, invalidEntities);
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
        spdlog::error("Missing a root element! ('" + path + "')");
        return s;
    }
    json& root = data.items().begin().value();
    if (!root.is_object()) {
        spdlog::error("Root must be an object! ('" + path + "')");
        return s;
    }
    s.id = data.items().begin().key();
    int invalidEntities = 0;
    if (root.contains(STAGE_JSON_ENTITIES_KEY)) {
        json& entities = root[STAGE_JSON_ENTITIES_KEY];
        if (entities.is_object())
            s.entities = ParseEntities(entities, false, &invalidEntities);
        else
            spdlog::warn("'" + std::string(STAGE_JSON_ENTITIES_KEY) + "' must be an object! ('" + path + "')");
    }
    if (root.contains(STAGE_JSON_ENTITIES_AUTO_KEY)) {
        json& autoEntities = root[STAGE_JSON_ENTITIES_AUTO_KEY];
        if (autoEntities.is_array()) {
            auto& parsedEntities = ParseEntities(autoEntities, true, &invalidEntities);
            s.entities.insert(s.entities.end(), parsedEntities.begin(), parsedEntities.end());
        }
        else
            spdlog::warn("'" + std::string(STAGE_JSON_ENTITIES_AUTO_KEY) + "' must be an array! ('" + path + "')");
    }
    if (invalidEntities > 0) {
        spdlog::warn(std::to_string(invalidEntities) + " invalid entities! ('" + path + "')");
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
        }
        else {
            bool hasEntityAlready = game->GetEntityManager().CountEntities(e.id) > 0;
            Entity& entity = game->GetEntityManager()[e.id];
            entity.OverrideComponentValues(e);
            if (!hasEntityAlready) {
                s.instantiatedEntities.insert(entity.GetHash());
                entity.Start();
            }
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
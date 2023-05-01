#include "core/stage/stage.h"

#include <fstream>
#include <unordered_map>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

#include "core/game.h"
#include "core/entity/entitymanager.h"

using json = nlohmann::json;

std::vector<Stage::Stage> stages;

#define STAGE_JSON_ENTITIES_KEY "entities"

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

std::vector<Entity> ParseEntities(const json& entities, int* invalidEntities = nullptr) {
    std::vector<Entity> parsedEntities;
    for (auto& e : entities) {
        if (e.is_object()) {
            Entity entity;
            for (const auto&[ck, cv] : e.items()) {
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
        else if (e.is_array()) {
            std::vector<Entity> recursiveEntities = ParseEntities(e, invalidEntities);
            parsedEntities.insert(parsedEntities.end(), recursiveEntities.begin(), recursiveEntities.end());
        }
        else if (invalidEntities != nullptr) {
            *invalidEntities++;
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
    if (root.contains(STAGE_JSON_ENTITIES_KEY)) {
        json& entities = root[STAGE_JSON_ENTITIES_KEY];
        if (entities.is_array()) {
            int invalidEntities = 0;
            s.entities = ParseEntities(entities, &invalidEntities);
            if (invalidEntities > 0) {
                spdlog::warn("'" + std::string(STAGE_JSON_ENTITIES_KEY) + "' contains " + std::to_string(invalidEntities) + " invalid entities! ('" + path + "')");
            }
        }
        else {
            spdlog::warn("'" + std::string(STAGE_JSON_ENTITIES_KEY) + "' must be an array! ('" + path + "')");
        }
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
    auto sIt = std::find_if(stages.begin(), stages.end(), [&](const Stage& s) {
        return s.id == id;
    });
    if (sIt == stages.end())
        return false;
    const Stage& s = *sIt;

    for (const Entity& e : s.entities) {
        game->GetEntityManager().AddEntity(e);
    }
    spdlog::info("Loaded stage '" + id + "' (" + std::to_string(s.entities.size()) + " entities)");
    return true;
}

bool Stage::UnloadStage(const std::string& id) {
    return true;
}

void Stage::UnloadAllStages() {

}
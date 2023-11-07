#pragma once
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <opengl.h>
#include <nlohmann/json.hpp>

#include "entity/entity.h"

namespace Stage {
    struct Stage {
        std::string id;
        // additional game-specific data
        nlohmann::json data;

        std::list<Entity> entities;
        std::unordered_set<size_t> instantiatedEntities;
    };

    // Returns a vector of all loaded stage IDs with the most recent being at index 0
    const std::vector<std::string>& GetLoadedStages();
    const Stage& GetStage(const std::string&);
    void AddStageFromFile(const std::string&);
    void AddStage(Stage&);
    bool LoadStage(const std::string&);
    bool UnloadStage(const std::string&);
    void UnloadAllStages();
}
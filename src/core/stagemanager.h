#pragma once
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <opengl.h>

#include "entity/entity.h"

namespace Stage {
    struct Stage {
        std::string id;
        std::list<Entity> entities;
        std::unordered_set<size_t> instantiatedEntities;
    };

    // Returns a vector of all loaded stage IDs with the most recent being at index 0
    const std::vector<std::string>& GetLoadedStages();
    void AddStageFromFile(const std::string&);
    void AddStage(Stage&);
    bool LoadStage(const std::string&);
    bool UnloadStage(const std::string&);
    void UnloadAllStages();
}
#pragma once
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <opengl.h>
#include <nlohmann/json.hpp>

#include "entity/entity.h"

struct Stage {
    std::string id;
    // additional game-specific data
    nlohmann::json data;

    std::list<Entity> entities;
    std::unordered_set<size_t> instantiatedEntities;
};

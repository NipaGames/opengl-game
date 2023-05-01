#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <opengl.h>
#include "core/entity/entity.h"

namespace Stage {
struct Stage {
    std::string id;
    std::vector<Entity> entities;
    glm::vec3 spawn = glm::vec3(0.0f);
};

Stage ReadStageFromFile(const std::string&);
void AddStage(const Stage&);
bool LoadStage(const std::string&);
bool UnloadStage(const std::string&);
void UnloadAllStages();
}
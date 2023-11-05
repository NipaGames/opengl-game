#include "stagemanager.h"

#include <fstream>
#include <unordered_map>
#include <spdlog/spdlog.h>

#include "game.h"
#include "entity/entitymanager.h"
#include "io/files/stage.h"

std::vector<Stage::Stage> stages;
std::vector<std::string> loadedStages;

void Stage::AddStageFromFile(const std::string& path) {
    Serializer::StageSerializer serializer(path);
    AddStage(serializer.GetStage());
}

void Stage::AddStage(Stage& stage) {
    if (!stage.id.empty())
        stages.emplace_back(stage);
}

const Stage::Stage& Stage::GetStage(const std::string& id) {
    return *std::find_if(stages.begin(), stages.end(), [&](const Stage& s) { return s.id == id; });
}

bool Stage::LoadStage(const std::string& id) {
    if (stages.empty())
        return false;
    auto sIt = std::find_if(stages.begin(), stages.end(), [&](const Stage& s) { return s.id == id; });
    if (sIt == stages.end())
        return false;
    Stage& s = *sIt;
    s.instantiatedEntities.clear();

    for (const Entity& e : s.entities) {
        if (e.id.empty()) {
            Entity& instantiated = GAME->GetEntityManager().AddEntity(e);
            s.instantiatedEntities.insert(instantiated.GetHash());
            instantiated.Start();
        }
        else {
            bool hasEntityAlready = GAME->GetEntityManager().CountEntities(e.id) > 0;
            Entity& entity = GAME->GetEntityManager()[e.id];
            entity.OverrideComponentValues(e);
            if (!hasEntityAlready) {
                s.instantiatedEntities.insert(entity.GetHash());
            }
            entity.Start();
        }
    }
    loadedStages.insert(loadedStages.begin(), s.id);
    spdlog::info("Loaded stage '" + id + "' (" + std::to_string(s.entities.size()) + " entities modified)");
    GAME->GetRenderer().UpdateLighting();
    GAME->GetRenderer().UpdateFrustum();
    return true;
}

bool Stage::UnloadStage(const std::string& id) {
    auto idIt = std::find(loadedStages.begin(), loadedStages.end(), id);
    if (idIt == loadedStages.end())
        return false;
    auto sIt = std::find_if(stages.begin(), stages.end(), [&](const Stage& s) { return s.id == id; });
    const Stage& s = *sIt;
    for (size_t hash : s.instantiatedEntities) {
        GAME->GetEntityManager().RemoveEntity(hash);
    }
    loadedStages.erase(idIt);
    GAME->GetRenderer().UpdateLighting();
    GAME->GetRenderer().UpdateFrustum();
    return true;
}

void Stage::UnloadAllStages() {
    for (int i = 0; i < loadedStages.size(); i++) {
        UnloadStage(loadedStages.at(i--));
    }
}

const std::vector<std::string>& Stage::GetLoadedStages() {
    return loadedStages;
}
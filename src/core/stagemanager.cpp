#include "stagemanager.h"

#include <fstream>
#include <unordered_map>
#include <spdlog/spdlog.h>

#include <core/game.h>
#include <core/entity/entitymanager.h>
#include <core/io/files/stage.h>

std::vector<Stage::Stage> stages;
std::vector<std::string> loadedStages;

void Stage::AddStageFromFile(const std::string& path) {
    Serializer::StageSerializer serializer;
    serializer.Serialize(path);
    AddStage(serializer.GetStage());
}

void Stage::AddStage(Stage& stage) {
    if (!stage.id.empty())
        stages.push_back(std::move(stage));
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
    const Stage& s = *sIt;
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
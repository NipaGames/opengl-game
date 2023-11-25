#include "stage.h"

#include <fstream>
#include <unordered_map>
#include <spdlog/spdlog.h>

#include "game.h"
#include "entity/entitymanager.h"
#include "io/files/stage.h"
#include "io/resourcemanager.h"

std::optional<Stage> Resources::StageManager::LoadResource(const std::fs::path& path) {
    Serializer::StageSerializer serializer;
    serializer.UseBlueprints(blueprints_);
    serializer.SerializeFile(path.generic_string());
    if (!serializer.Success())
        return std::nullopt;
    Stage s = serializer.GetStage();
    if (s.id.empty())
        return std::nullopt;
    SetItemID(s.id);
    return s;
}

bool Resources::StageManager::LoadStage(const std::string& id) {
    if (items_.empty())
        return false;
    if (items_.count(id) == 0)
        return false;
    Stage& s = items_.at(id);
    s.instantiatedEntities.clear();

    for (const Entity& e : s.entities) {
        if (e.id.empty()) {
            Entity& instantiated = GAME->GetEntityManager().CreateEntity();
            instantiated.OverrideComponentValues(e);
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
    loadedStages_.insert(loadedStages_.begin(), s.id);
    spdlog::info("Loaded stage '" + id + "' (" + std::to_string(s.entities.size()) + " entities modified)");
    GAME->GetRenderer().UpdateLighting();
    GAME->GetRenderer().UpdateFrustum();
    return true;
}

bool Resources::StageManager::UnloadStage(const std::string& id) {
    auto idIt = std::find(loadedStages_.begin(), loadedStages_.end(), id);
    if (idIt == loadedStages_.end())
        return false;
    const Stage& s = items_[id];
    for (size_t hash : s.instantiatedEntities) {
        GAME->GetEntityManager().RemoveEntity(hash);
    }
    loadedStages_.erase(idIt);
    GAME->GetRenderer().UpdateLighting();
    GAME->GetRenderer().UpdateFrustum();
    return true;
}

void Resources::StageManager::UnloadAllStages() {
    for (int i = 0; i < loadedStages_.size(); i++) {
        UnloadStage(loadedStages_.at(i--));
    }
}

const std::vector<std::string>& Resources::StageManager::GetLoadedStages() {
    return loadedStages_;
}

void Resources::StageManager::UseBlueprints(Serializer::BlueprintSerializer* blueprints) {
    blueprints_ = blueprints;
}
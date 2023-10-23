#include "entitymanager.h"

#include <algorithm>

Entity& EntityManager::CreateEntity(const std::string& id) {
    entities_.emplace_back(std::make_unique<Entity>(id));
    return *entities_.back();
}

Entity& EntityManager::AddEntity(Entity e) {
    entities_.push_back(std::make_unique<Entity>(e));
    return *entities_.back();
}

void EntityManager::RemoveEntities(const std::string& id) {
    entities_.erase(std::remove_if(entities_.begin(), entities_.end(), [&](const auto& e) { return e->id == id; }), entities_.end());
}

void EntityManager::RemoveEntity(size_t hash) {
    entities_.erase(std::remove_if(entities_.begin(), entities_.end(), [&](const auto& e) { return e->GetHash() == hash; }), entities_.end());
}

size_t EntityManager::CountEntities(const std::string& id) {
    return std::count_if(entities_.begin(), entities_.end(), [&](const auto& e) { return e->id == id; });
}

size_t EntityManager::CountEntities() {
    return entities_.size();
}

bool EntityManager::HasEntity(size_t hash) {
    // every entity has an unique hash anyway so no need to check if count > 1
    return std::count_if(entities_.begin(), entities_.end(), [&](const auto& e) { return e->GetHash() == hash; }) == 1;
}

Entity& EntityManager::GetEntity(const std::string& id) {
     return **std::find_if(entities_.begin(), entities_.end(), [&](const auto& e) { return e->id == id; });
}

Entity& EntityManager::operator[](const std::string& id) {
    auto it = std::find_if(entities_.begin(), entities_.end(), [&](const auto& e) { return e->id == id; });
    if (it != entities_.end())
        return **it;
    else
        return CreateEntity(id);
}
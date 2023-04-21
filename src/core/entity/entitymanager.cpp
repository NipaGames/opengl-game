#include "core/entity/entitymanager.h"

Entity& EntityManager::CreateEntity() {
    entities_.emplace_back();
    return entities_.back();
}

Entity& EntityManager::AddEntity(Entity e) {
    entities_.push_back(e);
    return entities_.back();
}
#include "core/entity/entitymanager.h"

Entity& EntityManager::CreateEntity(const std::string& id) {
    entities_.emplace_back(id);
    return entities_.back();
}

Entity& EntityManager::AddEntity(Entity e) {
    entities_.push_back(e);
    return entities_.back();
}

Entity& EntityManager::operator[](const std::string& id) {
    for (auto& e : entities_) {
        if (e.id == id)
            return e;
    }
    return CreateEntity(id);
}
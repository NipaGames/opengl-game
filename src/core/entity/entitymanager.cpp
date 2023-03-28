#include "core/entity/entitymanager.h"

Entity& EntityManager::CreateEntity() {
    entities_.emplace_back();
    return entities_.back();
}
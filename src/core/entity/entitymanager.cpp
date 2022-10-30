#include "core/entity/entitymanager.h"

Entity& EntityManager::CreateEntity() {
    entities_.push_back(Entity());
    return entities_.back();
}
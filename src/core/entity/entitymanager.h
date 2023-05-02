#pragma once

#include <list>

#include "core/entity/entity.h"

class EntityManager {
private:
    std::list<Entity> entities_;
public:
    Entity& CreateEntity(const std::string& = "");
    Entity& AddEntity(Entity);
    // also creates a new entity if it doesn't exist
    Entity& operator[](const std::string& id);
    friend class Game;
};
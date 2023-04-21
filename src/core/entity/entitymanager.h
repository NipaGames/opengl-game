#pragma once

#include <list>

#include "core/entity/entity.h"

class EntityManager {
private:
    std::list<Entity> entities_;
public:
    Entity& CreateEntity();
    Entity& AddEntity(Entity);
    friend class Game;
};
#pragma once

#include <vector>

#include "core/entity/entity.h"

class EntityManager {
private:
    std::vector<Entity> entities_;
public:
    Entity& CreateEntity();
    friend class Game;
};
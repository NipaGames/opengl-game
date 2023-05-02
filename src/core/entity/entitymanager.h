#pragma once

#include <list>

#include "core/entity/entity.h"

class EntityManager {
private:
    std::list<Entity> entities_;
public:
    Entity& CreateEntity(const std::string& = "");
    Entity& AddEntity(Entity);
    void RemoveEntities(const std::string&);
    void RemoveEntity(size_t);
    int CountEntities(const std::string&);
    bool HasEntity(size_t);
    // also creates a new entity if it doesn't exist
    Entity& operator[](const std::string& id);
    friend class Game;
};
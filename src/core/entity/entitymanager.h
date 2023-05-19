#pragma once

#include <list>

#include "entity.h"

class EntityManager {
private:
    std::list<std::unique_ptr<Entity>> entities_;
public:
    Entity& CreateEntity(const std::string& = "");
    Entity& AddEntity(Entity);
    void RemoveEntities(const std::string&);
    void RemoveEntity(size_t);
    size_t CountEntities(const std::string&);
    size_t CountEntities();
    bool HasEntity(size_t);
    // also creates a new entity if it doesn't exist
    Entity& operator[](const std::string& id);
    friend class Game;
};
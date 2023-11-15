#pragma once

#include <list>

#include "entity.h"

class EntityManager {
private:
    std::list<std::unique_ptr<Entity>> entities_;
public:
    Entity& CreateEntity(const std::string& = "");
    Entity& AddEntity(const Entity&);
    void RemoveEntities(const std::string&);
    void RemoveEntity(size_t);
    size_t CountEntities(const std::string&);
    size_t CountEntities();
    void ClearEntities();
    bool HasEntity(size_t);
    // also creates a new entity if it doesn't exist
    Entity& operator[](const std::string& id);
    Entity& GetEntity(const std::string& id);
    const std::list<std::unique_ptr<Entity>>& GetEntities();
    friend class Game;
};
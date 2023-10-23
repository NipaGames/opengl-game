#include "entity.h"

#include <unordered_set>
#include <core/gamewindow.h>
#include <core/graphics/renderer.h>
#include <core/game.h>

Entity::Entity(const Entity& e) {
    hash_ = GenerateHash();
    CopyFrom(e);
}

Entity::Entity(Entity&& e) : 
    hash_(e.hash_),
    id(e.id),
    components_(e.components_), 
    transform(e.transform)
{
    for (auto c : components_) {
        c->parent = this;
    }
    e.components_.clear();
}

Entity::~Entity() {
    for (auto component : components_) {
        delete component;
    }
    components_.clear();
}

void Entity::CopyFrom(const Entity& e) {
    id = e.id;
    components_.clear();
    for (auto c : e.components_) {
        IComponent* c2 = c->Clone();
        c2->parent = this;
        components_.push_back(c2);
    }
    transform = GetComponent<Transform>();
}

void Entity::Start() {
    for (auto component : components_) {
        if (!component->hasStarted_)
            component->IStart();
        component->hasStarted_ = true;
    }
}

void Entity::Update() {
    for (auto component : components_) {
        component->IUpdate();
    }
}

void Entity::FixedUpdate() {
    for (auto component : components_) {
        component->IFixedUpdate();
    }
}

void Entity::Destroy(EntityManager& mgr) {
    mgr.RemoveEntity(hash_);
}

void Entity::Destroy() {
    Destroy(GAME->GetEntityManager());
}

std::unordered_set<size_t> usedHashes;
size_t Entity::GenerateHash() {
    size_t hash;
    do {
        hash = std::hash<int>{}(rand());
    }
    while (usedHashes.find(hash) != usedHashes.end());
    usedHashes.insert(hash);
    return hash;
}
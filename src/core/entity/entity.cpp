#include "core/entity/entity.h"

#include "core/gamewindow.h"
#include "core/graphics/renderer.h"

Entity::Entity(const Entity& e) {
    id = e.id;
    components_.clear();
    for (auto c : e.components_) {
        IComponent* c2 = c->Clone();
        c2->parent = this;
        components_.push_back(c2);
    }
    transform = GetComponent<Transform>();
}

Entity::Entity(Entity&& e) {
    id = e.id;
    components_ = e.components_;
    transform = e.transform;
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

void Entity::Start() {
    for (auto component : components_) {
        component->IStart();
    }
}

void Entity::Update() {
    for (auto component : components_) {
        component->IUpdate();
    }
}
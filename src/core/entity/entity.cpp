#include "core/entity/entity.h"

#include "core/gamewindow.h"
#include "core/graphics/renderer.h"

Entity::Entity() {
    transform = AddComponent<Transform>();
}

Entity::Entity(const Entity& e) {
    for (auto c : e.components_) {
        IComponent* newComponent = c->Clone();
        newComponent->parent = this;
        components_.push_back(newComponent);
    }
    transform = GetComponent<Transform>();
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
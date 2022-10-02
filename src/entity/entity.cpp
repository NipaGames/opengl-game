#include "entity/entity.h"

#include "gamewindow.h"
#include "graphics/renderer.h"

Entity::Entity() {
    transform = AddComponent<Transform>();
}

Entity::~Entity() {
    for (auto component : components_) {
        delete component;
    }
    components_.clear();
}

void Entity::Start() {
    for (auto component : components_) {
        component->Start();
    }
}

void Entity::Update() {
    for (auto component : components_) {
        component->Update();
    }
}
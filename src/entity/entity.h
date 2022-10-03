#pragma once

#include <opengl.h>

class Component;

#include "graphics/mesh.h"
#include "entity/component.h"
#include "entity/component/transform.h"

class Entity {
private:
    std::vector<Component*> components_;
public:
    Entity();
    virtual ~Entity();

    Transform* transform;

    void Start();
    void Update();
    template<typename C>
    C* const GetComponent() {
        for (int i = 0; i < components_.size(); i++) {
            C* c = dynamic_cast<C*>(components_[i]);
            if (c != nullptr)
                return c;
        }
        return nullptr;
    }
    template<typename C>
    std::vector<C* const> GetComponents() {
        std::vector<C* const> components;
        for (int i = 0; i < components_.size(); i++) {
            C* c = dynamic_cast<C*>(components_[i]);
            if (c != nullptr)
                components.push_back((C*) c);
        }
        return components;
    }

    template<typename C>
    C* AddComponent() {
        if constexpr(!std::is_base_of_v<Component, C>) {
            spdlog::error("{} doesn't derive from Component-class!", typeid(C).name());
            throw;
        }
        C* component = new C();
        ((Component*) component)->parent = this;
        components_.push_back((Component*) component);
        return component;
    }

    template<typename... C>
    void AddComponents() {
        ([&] { // i have no fucking idea how this and c++ lambdas in general work
            AddComponent<C>();
        } (), ...);
    }
};
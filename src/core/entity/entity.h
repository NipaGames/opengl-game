#pragma once

#include <opengl.h>

class IComponent;
template<class C>
class Component;

#include "core/graphics/mesh.h"
#include "core/entity/component.h"
#include "core/entity/component/transform.h"

class Entity {
private:
    std::vector<IComponent*> components_;
public:
    Entity();
    Entity(const Entity&);
    Entity(Entity&&);
    virtual ~Entity();

    Transform* transform;

    void Start();
    void Update();
    template<typename C>
    C* const GetComponent() {
        for (int i = 0; i < components_.size(); i++) {
            C* c = static_cast<C*>(components_[i]);
            if (c != nullptr)
                return c;
        }
        return nullptr;
    }
    template<typename C>
    void RemoveComponent() {
        for (auto it = components_.begin(); it != components_.end(); it++) {
            C* c = static_cast<C*>(*it);
            if (c != nullptr) {
                components_.erase(it);
                delete c;
                return;
            }
        }
    }
    template<typename... C>
    void RemoveComponents() {
        ([&] {
            RemoveComponent<C>();
        } (), ...);
    }
    template<typename C, typename... Args>
    C* AddComponent(Args... args) {
        C* component = new C((args)...);
        component->parent = this;
        components_.push_back((IComponent*) component);
        return component;
    }
};
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
    std::vector<C* const> GetComponents() {
        std::vector<C* const> components;
        for (int i = 0; i < components_.size(); i++) {
            C* c = static_cast<C*>(components_[i]);
            if (c != nullptr)
                components.push_back((C*) c);
        }
        return components;
    }

    template<typename C>
    C* AddComponent() {
        if constexpr(!std::is_base_of_v<Component<C>, C>) {
            spdlog::error("{} doesn't derive from Component-class!", typeid(C).name());
            throw;
        }
        C* component = new C();
        component->parent = this;
        components_.push_back((IComponent*) component);
        return component;
    }

    template<typename... C>
    void AddComponents() {
        ([&] { // i have no fucking idea how this and c++ lambdas in general work
            AddComponent<C>();
        } (), ...);
    }
};
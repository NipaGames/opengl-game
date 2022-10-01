#pragma once

#include <opengl.h>

#include "graphics/mesh.h"
#include "entity/component.h"
#include "entity/component/transform.h"

class Entity {
private:
    std::vector<Component*> components_;
public:
    Entity() { }
    virtual ~Entity();

    Transform transform;

    void Start();
    void Update();
    template<typename C>
    C* const GetComponent() {
        for (auto c : components_) {
            if constexpr(std::is_same_v<T, typeid(c)>)
                return c;
        }
        return nullptr;
    }
    template<typename C>
    std::vector<C* const> GetComponents() {
        std::vector<C* const> components;
        for (auto c : components_) {
            if constexpr(std::is_same_v<T, typeid(c)>)
                components.insert(c);
        }
        return components;
    }

    template<typename... C>
    void Create() {
        ([&] { // i have no fucking idea how this and c++ lambdas in general work
            if constexpr(!std::is_base_of<Component, C>::value) {
                spdlog::error("{} doesn't derive from Component-class!", typeid(C).name());
                throw;
            }
        } (), ...);
    }
};
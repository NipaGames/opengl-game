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
    Entity Entity::operator=(const Entity& e) {
        return Entity(e);
    }
    virtual ~Entity();

    Transform* transform;

    void Start();
    void Update();
    IComponent* const GetComponent(const type_info* type) {
        size_t hash = type->hash_code();
        for (auto c : components_) {
            if (c->typeHash == hash)
                return c;
        }
        return nullptr;
    }
    template<typename C>
    C* const GetComponent() {
        return static_cast<C*>(GetComponent(&typeid(C)));
    }
    IComponent* GetComponent(const std::string& name) {
        for (auto& c : IComponent::COMPONENT_TYPES_) {
            if (c.name == name)
                return GetComponent(c.type);
        }
        return nullptr;
    }
    template<typename C>
    void RemoveComponent() {
        size_t hash = typeid(C).hash_code();
        for (auto it = components_.begin(); it != components_.end(); ++it) {
            C* c = static_cast<C*>(*it);
            if (c->typeHash == hash) {
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
        C* c = new C((args)...);
        c->parent = this;
        c->typeHash = typeid(C).hash_code();
        components_.push_back((IComponent*) c);
        return c;
    }
    IComponent* AddComponent(const type_info* type, const ComponentData& data = ComponentData()) {
        int i = 0;
        for (;i < IComponent::COMPONENT_TYPES_.size(); i++) {
            if (IComponent::COMPONENT_TYPES_[i].type == type)
                break;
        }
        if (i == IComponent::COMPONENT_TYPES_.size())
            return nullptr;
        IComponent* c = IComponent::COMPONENT_TYPES_[i].initializer(data);
        c->parent = this;
        c->typeHash = type->hash_code();
        components_.push_back((IComponent*) c);
        return c;
    }
    IComponent* AddComponent(const std::string& name, const ComponentData& data = ComponentData()) {
        for (auto& c : IComponent::COMPONENT_TYPES_) {
            if (c.name == name)
                return AddComponent(c.type, data);
        }
        return nullptr;
    }
};
#pragma once

#include <opengl.h>

class IComponent;
template<class C>
class Component;

#include "component.h"
#include "transform.h"

class EntityManager;
class Entity {
private:
    std::vector<IComponent*> components_;
    size_t hash_;
    static size_t GenerateHash();
public:
    Entity(const std::string& id = "") : id(id) {
        hash_ = GenerateHash();
        AddComponent<Transform>();
    }
    Entity(const Entity&);
    Entity(Entity&&);
    const Entity& operator=(const Entity& e) {
        hash_ = e.hash_;
        CopyFrom(e);
        return *this;
    }
    virtual ~Entity();

    Transform* transform;
    std::string id;

    void Start();
    void Update();
    void FixedUpdate();
    void CopyFrom(const Entity&);
    void Destroy(EntityManager& mgr);
    void Destroy();
    size_t GetHash() const { return hash_; }
    IComponent* const GetComponent(const type_info* type) const {
        size_t hash = type->hash_code();
        for (auto c : components_) {
            if (c->typeHash == hash)
                return c;
        }
        return nullptr;
    }
    template<typename C>
    C* const GetComponent() const {
        return static_cast<C*>(GetComponent(&typeid(C)));
    }
    IComponent* GetComponent(const std::string& name) const {
        for (auto& c : IComponent::COMPONENT_TYPES_) {
            if (c.name == name)
                return GetComponent(c.type);
        }
        return nullptr;
    }
    IComponent* GetComponent(size_t hash) const {
        for (auto& c : IComponent::COMPONENT_TYPES_) {
            if (c.type->hash_code() == hash)
                return GetComponent(c.type);
        }
        return nullptr;
    }
    void RemoveComponent(size_t hash) {
        for (auto it = components_.begin(); it != components_.end(); ++it) {
            IComponent* c = *it;
            if (c->typeHash == hash) {
                components_.erase(it);
                delete c;
                return;
            }
        }
    }
    template<typename C>
    void RemoveComponent() {
        RemoveComponent(typeid(C).hash_code());
    }
    template<typename... C>
    void RemoveComponents() {
        ([&] {
            RemoveComponent<C>();
        } (), ...);
    }
    void RemoveComponent(const std::string& name) {
        for (auto& c : IComponent::COMPONENT_TYPES_) {
            if (c.name == name)
                RemoveComponent(c.type->hash_code());
        }
    }
    template<typename C, typename... Args>
    C* AddComponent(Args... args) {
        C* c = new C((args)...);
        c->parent = this;
        c->typeHash = typeid(C).hash_code();
        components_.push_back(static_cast<IComponent*>(c));
        if (typeid(C) == typeid(Transform))
            transform = dynamic_cast<Transform*>(c);
        return c;
    }
    IComponent* AddComponent(const type_info*, const ComponentData& = ComponentData());
    IComponent* AddComponent(const std::string&, const ComponentData& = ComponentData());
    IComponent* AddComponent(size_t, const ComponentData& = ComponentData());
    std::vector<std::string> ListComponentNames() const;
    void OverrideComponentValues(const Entity& e);
};
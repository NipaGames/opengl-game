#pragma once

#include <opengl.h>

class IComponent;
template<class C>
class Component;

#include "component.h"
#include "transform.h"
#include <core/graphics/mesh.h>

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
        if (type == &typeid(Transform))
            transform = static_cast<Transform*>(c);
        return c;
    }
    IComponent* AddComponent(const std::string& name, const ComponentData& data = ComponentData()) {
        for (auto& c : IComponent::COMPONENT_TYPES_) {
            if (c.name == name)
                return AddComponent(c.type, data);
        }
        return nullptr;
    }
    IComponent* AddComponent(size_t hash, const ComponentData& data = ComponentData()) {
        for (auto& c : IComponent::COMPONENT_TYPES_) {
            if (c.type->hash_code() == hash)
                return AddComponent(c.type, data);
        }
        return nullptr;
    }
    void OverrideComponentValues(const Entity& e) {
        for (auto c : e.components_) {
            IComponent* mc = GetComponent(c->typeHash);
            if (mc == nullptr)
                mc = AddComponent(c->typeHash, c->data);
            else {
                for (auto&[k, v] : c->data.vars) {
                    v->CloneValuesTo(mc->data.vars[k]);
                }
            }
        }
        Start();
    }
};
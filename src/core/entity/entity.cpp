#include "entity.h"

#include <unordered_set>
#include <core/gamewindow.h>
#include <core/graphics/renderer.h>
#include <core/game.h>

Entity::Entity(const Entity& e) {
    hash_ = GenerateHash();
    CopyFrom(e);
}

Entity::Entity(Entity&& e) : 
    hash_(e.hash_),
    id(e.id),
    components_(e.components_), 
    transform(e.transform)
{
    for (IComponent* c : components_) {
        c->parent = this;
    }
    e.components_.clear();
}

Entity::~Entity() {
    for (IComponent* component : components_) {
        delete component;
    }
    components_.clear();
}

void Entity::CopyFrom(const Entity& e) {
    id = e.id;
    components_.clear();
    for (IComponent* c : e.components_) {
        IComponent* c2 = c->Clone();
        c2->parent = this;
        components_.push_back(c2);
    }
    transform = GetComponent<Transform>();
}

IComponent* Entity::AddComponent(const type_info* type, const ComponentData& data) {
    IComponent* c = IComponent::CreateComponent(type, data);
    if (c == nullptr)
        return nullptr;
    c->parent = this;
    components_.push_back((IComponent*) c);
    if (type == &typeid(Transform))
        transform = static_cast<Transform*>(c);
    return c;
}
IComponent* Entity::AddComponent(const std::string& name, const ComponentData& data) {
    for (auto& c : IComponent::COMPONENT_TYPES_) {
        if (c.name == name)
            return AddComponent(c.type, data);
    }
    return nullptr;
}
IComponent* Entity::AddComponent(size_t hash, const ComponentData& data) {
    for (auto& c : IComponent::COMPONENT_TYPES_) {
        if (c.type->hash_code() == hash)
            return AddComponent(c.type, data);
    }
    return nullptr;
}

void Entity::OverrideComponentValues(const Entity& e) {
    for (auto c : e.components_) {
        IComponent* mc = GetComponent(c->typeHash);
        if (mc == nullptr)
            AddComponent(c->typeHash, c->data);
        else {
            for (const auto&[k, v] : c->data.vars) {
                v->CloneValuesTo(mc->data.vars[k]);
            }
        }
    }
}

void Entity::Start() {
    for (IComponent* component : components_) {
        if (!component->hasStarted_)
            component->IStart();
        component->hasStarted_ = true;
    }
}

void Entity::Update() {
    for (IComponent* component : components_) {
        if (!component->hasHadFirstUpdate_) {
            component->IFirstUpdate();
            component->hasHadFirstUpdate_ = true;
        }
        component->IUpdate();
    }
}

void Entity::FixedUpdate() {
    for (auto component : components_) {
        component->IFixedUpdate();
    }
}

void Entity::Destroy(EntityManager& mgr) {
    mgr.RemoveEntity(hash_);
}

void Entity::Destroy() {
    Destroy(GAME->GetEntityManager());
}

std::vector<std::string> Entity::ListComponentNames() const {
    std::vector<std::string> names;
    for (const IComponent* c : components_) {
        for (const ComponentType& t : IComponent::COMPONENT_TYPES_) {
            if (t.type->hash_code() == c->typeHash) {
                names.push_back(t.name);
                break;
            }
        }
    }
    return names;
}

std::unordered_set<size_t> usedHashes;
size_t Entity::GenerateHash() {
    size_t hash;
    do {
        hash = std::hash<int>{}(rand());
    }
    while (usedHashes.find(hash) != usedHashes.end());
    usedHashes.insert(hash);
    return hash;
}
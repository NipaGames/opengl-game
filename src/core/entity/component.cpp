#include "component.h"
#include "entity.h"

IComponent::IComponent(const IComponent& c) {
    hasStarted_ = c.hasStarted_;
    hasHadFirstUpdate_ = c.hasHadFirstUpdate_;
    for (const auto& [k, v] : c.data.vars) {
        if (data.vars.count(k) != 0)
            v->CloneValuesTo(data.vars.at(k));
    }
    data = c.data;
    parent = c.parent;
    typeHash = c.typeHash;
}

IComponent* IComponent::CreateComponent(const type_info* type, const ComponentData& data) {
    auto it = std::find_if(COMPONENT_TYPES_.begin(), COMPONENT_TYPES_.end(), [&](const auto& t) { return t.type == type; });
    if (it == COMPONENT_TYPES_.end())
        return nullptr;
    IComponent* c = it->initializer(data);
    c->typeHash = type->hash_code();
    return c;
}
IComponent* IComponent::CreateComponent(const std::string& name, const ComponentData& data) {
    auto it = std::find_if(COMPONENT_TYPES_.begin(), COMPONENT_TYPES_.end(), [&](const auto& t) { return t.name == name; });
    if (it == COMPONENT_TYPES_.end())
        return nullptr;
    return CreateComponent(it->type, data);
}
IComponent* IComponent::CreateComponent(size_t hash, const ComponentData& data) {
    auto it = std::find_if(COMPONENT_TYPES_.begin(), COMPONENT_TYPES_.end(), [&](const auto& t) { return t.type->hash_code() == hash; });
    if (it == COMPONENT_TYPES_.end())
        return nullptr;
    return CreateComponent(it->type, data);
}
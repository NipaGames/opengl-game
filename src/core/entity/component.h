#pragma once

#include <iostream>
#include <sstream>
#include <unordered_map>
#include <spdlog/spdlog.h>

class IComponent {
friend class Entity;
private:
    static inline std::unordered_map<const type_info*, IComponent*(*)()> COMPONENT_INITIALIZERS_;
public:
    Entity* parent;
    virtual IComponent* Clone() const = 0;
    virtual ~IComponent() = default;
    virtual void IStart() = 0;
    virtual void IUpdate() = 0;
    static IComponent* CreateInstance() { return nullptr; }
    template<typename C>
    static bool RegisterComponent() {
        const type_info* type = &typeid(C);
        COMPONENT_INITIALIZERS_[type] = *C::CreateInstance;
        std::string name = std::string(type->name());
        size_t i = name.find(" ");
        if (i != std::string::npos)
            name = name.substr(i + 1);
        std::cout << "<Registered component " << name << ">" << std::endl;
        return true;
    }
};
// all components that aren't straightly derived from Component will need this
#define REGISTER_COMPONENT(C) inline bool _##C_isRegistered = IComponent::RegisterComponent<C>()

template<class Derived>
class Component : public IComponent {
protected:
    inline static bool _isRegistered = IComponent::RegisterComponent<Derived>();
public:
    virtual ~Component() {  }
    virtual IComponent* Clone() const override {
        return new Derived(static_cast<Derived const&>(*this));
    }
    void IStart() override { static_cast<Derived*>(this)->Start(); }
    void IUpdate() override { static_cast<Derived*>(this)->Update(); }

    virtual void Start() { }
    virtual void Update() { }
    static IComponent* CreateInstance() { 
        return new Derived();
    }
};
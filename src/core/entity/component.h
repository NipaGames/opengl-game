#pragma once
#include <functional>
#include <iostream>
#include <unordered_map>

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
    static void RegisterComponent() {
        COMPONENT_INITIALIZERS_[&typeid(C)] = *C::CreateInstance;
    }
};
template<class Derived>
class Component : public IComponent {
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
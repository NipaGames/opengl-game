#pragma once
#include <iostream>

class Entity; // forward declaration, can't #include "core/entity/entity.h"
class IComponent {
public:
    Entity* parent;
    virtual IComponent* Clone() const = 0;
    virtual ~IComponent() = default;
    virtual void IStart() = 0;
    virtual void IUpdate() = 0;
};
template<class Derived>
class Component : public IComponent {
public:
    virtual ~Component() { }
    virtual IComponent* Clone() const override {
        return new Derived(static_cast<Derived const&>(*this));
    }
    void IStart() override { static_cast<Derived*>(this)->Start(); }
    void IUpdate() override { static_cast<Derived*>(this)->Update(); }

    virtual void Start() { }
    virtual void Update() { }
};
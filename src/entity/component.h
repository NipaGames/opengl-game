#pragma once

class Entity; // forward declaration, can't #include "entity/entity.h"
class Component {
public:
    Entity* parent;
    virtual ~Component() { }
    virtual void Start() { }
    virtual void Update() { }
};
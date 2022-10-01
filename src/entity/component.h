#pragma once

class Component {
public:
    virtual ~Component() { }
    virtual void Start() { }
    virtual void Update() { }
};
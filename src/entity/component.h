#pragma once

class Component {
public:
    virtual ~Component() { }
    void AddComponent() { }
    virtual void Start() { }
    virtual void Update() { }
};
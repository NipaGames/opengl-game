#pragma once

#include "entity/component.h"

class PlayerController : public Component {
private:
    void OnMouseMove();
    double lastMouseX_, lastMouseY_;
    float speed = 15;
public:
    void Start() override;
    void Update() override;
};
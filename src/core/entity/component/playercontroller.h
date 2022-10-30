#pragma once

#include "core/entity/component.h"

class PlayerController : public Component<PlayerController> {
private:
    void OnMouseMove();
    double lastMouseX_, lastMouseY_;
    float speed = 15;
public:
    void Start();
    void Update();
};
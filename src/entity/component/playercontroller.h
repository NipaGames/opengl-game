#pragma once

#include "entity/component.h"

class PlayerController : public Component {
private:
    void OnMouseMove();
    double lastMouseX_, lastMouseY_;
public:
    void Start() override;
    void Update() override;
};
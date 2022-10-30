#pragma once

#include "core/entity/component.h"

class PlayerController : public Component<PlayerController> {
private:
    void OnMouseMove();
    float speed = 15;
public:
    void Start();
    void Update();
};
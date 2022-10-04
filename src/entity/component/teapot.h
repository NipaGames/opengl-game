#pragma once

#include "gamewindow.h"
#include "entity/component.h"
#include "entity/entity.h"

class Teapot : public Component {
public:
    void Update() override {
        parent->transform->position.y += (pow(parent->transform->position.y / 2 + 1.0f, 2) / 5) * (float) game.GetDeltaTime();
    }
};
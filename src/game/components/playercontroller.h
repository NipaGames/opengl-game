#pragma once

#include <opengl.h>
#include "core/entity/component.h"

class PlayerController : public Component<PlayerController> {
private:
    void OnMouseMove();
    bool isFlying_ = false;
public:
    void Start();
    void Update();

    float speed = 10;
    float mass = 4;
    glm::vec3 velocity;
};
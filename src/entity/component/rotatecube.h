#pragma once

#include "entity/component.h"
#include "entity/entity.h"

#include <opengl.h>

class RotateCube : public Component {
public:
    void Update() override {
        glm::mat4 transform(1.0f);
        transform = glm::rotate(transform, (float) game.GetDeltaTime(), glm::vec3(0.0f, 1.0f, 0.0f));
        transform = glm::translate(transform, parent->transform->position);
        transform *= glm::toMat4(parent->transform->rotation);
        parent->transform->position = glm::vec3(transform[3]);
    }
};
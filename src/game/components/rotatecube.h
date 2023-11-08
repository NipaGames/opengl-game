#pragma once

#include <core/entity/component.h>
#include <core/entity/entity.h>
#include <core/game.h>

#include <opengl.h>

class RotateCube : public Component<RotateCube> {
private:
    float speed_ = -3.0f;
public:
    void Update() {
        glm::mat4 transform(1.0f);
        transform = glm::rotate(transform, (float) GAME->GetDeltaTime() * speed_, glm::vec3(0.0f, 1.0f, 0.0f));
        transform = glm::translate(transform, parent->transform->position);
        transform *= glm::toMat4(parent->transform->rotation);
        parent->transform->position = glm::vec3(transform[3]);
        glm::vec3 eulers = glm::vec3(glm::radians(-90.0f), glm::atan2(parent->transform->position.x, parent->transform->position.z) - M_PI, 0);
        parent->transform->rotation = eulers;
    }
};
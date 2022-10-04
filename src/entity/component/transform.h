#pragma once

#include "entity/component.h"

#include <opengl.h>

class Transform : public Component {
public:
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 size = glm::vec3(1.0f);
    glm::quat rotation = glm::quat(0.0f, 0.0f, 0.0f, 0.0f);
};
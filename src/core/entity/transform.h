#pragma once

#include "component.h"

#include <opengl.h>

class Transform : public Component<Transform> {
public:
    DEFINE_COMPONENT_DATA_VALUE(glm::vec3, position, glm::vec3(0.0f));
    DEFINE_COMPONENT_DATA_VALUE(glm::vec3, size, glm::vec3(1.0f));
    DEFINE_COMPONENT_DATA_VALUE(glm::quat, rotation, glm::quat(0.0f, 0.0f, 0.0f, 0.0f));
};
#include "ship.h"
#define _USE_MATH_DEFINES
#include <math.h>

void Ship::FixedUpdate() {
    float factor = (float) sin(glfwGetTime() * M_PI * speed);
    parent->transform->position.y = factor * height;
    glm::vec3 eulers = glm::eulerAngles(parent->transform->rotation);
    eulers.z = (float) M_PI + factor * glm::radians(maxRotation);
    parent->transform->rotation = eulers;
}
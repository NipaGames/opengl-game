#include "ship.h"
#define _USE_MATH_DEFINES
#include <math.h>

void Ship::FixedUpdate() {
    parent->transform->position.y = (float) sin(glfwGetTime() * M_PI * speed) * height;
}
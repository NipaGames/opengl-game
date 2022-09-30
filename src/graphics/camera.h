#pragma once

#include <glm/gtc/matrix_transform.hpp>

class Camera {
public:
    float fov = 60.0f;
    float yaw = 0.0f;
    float pitch = 0.0f;
    glm::vec3 front = glm::vec3(0.0f, 0.0f, 1.0f);
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 pos = glm::vec3(0.0f);
    glm::mat4 projectionMatrix;
};
#pragma once

#include <opengl.h>

struct PointLight {
    glm::vec3 pos;
    glm::vec3 color = glm::vec3(1.0);
    float range = 20.0f;
    float intensity = 1.0f;
};

struct DirectionalLight {
    glm::vec3 dir;
    glm::vec3 color = glm::vec3(1.0);
    float intensity = 1.0f;
};

struct Spotlight {
    glm::vec3 pos;
    glm::vec3 dir;
    glm::vec3 color = glm::vec3(1.0);
    float cutOffMin;
    float cutOffMax;
    float range = 20.0f;
    float intensity = 1.0f;
};
#include "sanctuarysun.h"
#include <core/game.h>
#include <core/graphics/component/meshrenderer.h>

constexpr float PI = 3.14159265358979323846f;

void SanctuarySun::Start() {
    startTime_ = (float) glfwGetTime();
    light_ = parent->GetComponent<Lights::DirectionalLight>();
}

void SanctuarySun::FirstUpdate() {
    // hell yeah
    mountainMaterial_ = GAME->GetEntityManager().GetEntity("sanctuary-mountains").GetComponent<MeshRenderer>()->meshes.at(0)->material;
}

void SanctuarySun::FixedUpdate() {
    float time = (float) glfwGetTime() - startTime_;
    float cyclePos = 2.0f * PI * time / dayNightCycleLength_;
    float dirZ = std::cosf(cyclePos);
    float dirY = std::sinf(cyclePos) + .25f;
    float brightness = std::max(std::min(dirY, 1.0f), 0.0f);
    glm::vec3 skyColor = glm::vec3(.5f) * brightness;

    light_->dir = { 0.0f, dirY, dirZ };
    light_->intensity = brightness;
    light_->ApplyForAllShaders();

    mountainMaterial_->SetShaderUniform("fog.color", skyColor);
    GAME->GetRenderer().skyboxColor = skyColor;
}
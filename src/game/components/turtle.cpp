#include "turtle.h"

#include <core/graphics/component/meshrenderer.h>
#include <core/game.h>

Turtle::~Turtle() {
    if (light_ != nullptr) {
        GAME->GetRenderer().RemoveLight(light_);
        light_->isAdded = false;
        GAME->GetRenderer().UpdateLighting();
        light_ = nullptr;
    }
}

void Turtle::Interpolate(float t) {
    if (t < .2f) {
        for (const auto& material : materials_) {
            float t1 = t * 5.0f;
            material->SetShaderUniform("opacity", t1);
            if (light_ != nullptr) {
                light_->intensity = t1 * 2.0f;
            }
        }
    }
    else {
        float t2 = ((t - .2f) * 1.25f);
        parent->transform->position.y = baseY_ - t2 * drop;
    }
    if (light_ != nullptr) {
        light_->ApplyForAllShaders();
    }
}

void Turtle::End() {
    for (const auto& material : materials_) {
        material->SetShaderUniform("opacity", 1.0f);
    }
}

void Turtle::FirstUpdate() {
    animationLength = 5.0f;
    light_ = parent->AddComponent<Lights::PointLight>();
    light_->intensity = 0.0f;
    light_->range = 25.0f;
    light_->Start();
    light_->color = glm::vec3(1.0f, 0.0f, 0.0f);
    GAME->GetRenderer().UpdateLighting();
    
    const auto& meshes = parent->GetComponent<MeshRenderer>()->meshes;
    std::transform(meshes.begin(), meshes.end(), std::back_inserter(materials_), [](const auto& m) { return m->material; });

    baseY_ = parent->transform->position.y;
    Interpolate(0.0f);
    AnimationComponent::Play();
}
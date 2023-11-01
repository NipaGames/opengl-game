#include "rotationanimation.h"
#include <core/game.h>

void RotationAnimationComponent::RotationAnimation(std::string entityId) {
    if (GAME->GetEntityManager().CountEntities(entityId) == 0) {
        spdlog::warn("Entity '{}' not found!", entityId);
        return;
    }
    RotationAnimationComponent* animationComponent = GAME->GetEntityManager().GetEntity(entityId).GetComponent<RotationAnimationComponent>();
    if (animationComponent == nullptr) {
        spdlog::warn("Entity '{}' does not have an animation component!", entityId);
        return;
    }
    animationComponent->Play();
}

void RotationAnimationComponent::Start() {
    meshRenderer_ = parent->GetComponent<MeshRenderer>();
    if (transformAtStart) {
        glm::mat4 baseTransform = parent->GetComponent<MeshRenderer>()->meshes.at(meshTransforms[0])->transformMatrix;
        startTransform_ = baseTransform;
        glm::mat4 transform = glm::rotate(baseTransform, glm::radians(-angle), rotationAxis);
        for (int meshIndex : meshTransforms) {
            meshRenderer_->meshes.at(meshIndex)->transformMatrix = transform;
        }
    }
}

void RotationAnimationComponent::Update() {
    if (isPlaying_) {
        if (glfwGetTime() < animationStart_)
            return;
        float x = (float) glfwGetTime() - animationStart_;
        float rotation = -angle + (x / animationLength) * 2.0f * angle;
        if (x >= animationLength) {
            rotation = angle;
            isPlaying_ = false;
        }
        glm::mat4 transform = glm::rotate(startTransform_, glm::radians(rotation), rotationAxis);
        for (int meshIndex : meshTransforms) {
            meshRenderer_->meshes.at(meshIndex)->transformMatrix = transform;
        }
    }
}

void RotationAnimationComponent::Play() {
    if (isPlaying_)
        return;
    isPlaying_ = true;
    if (!transformAtStart)
        startTransform_ = parent->GetComponent<MeshRenderer>()->meshes.at(meshTransforms[0])->transformMatrix;
    animationStart_ = (float) glfwGetTime() + animationDelay;
}
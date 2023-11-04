#include "animationcomponent.h"
#include <core/game.h>

void AnimationComponent::PlayAnimation(std::string entityId, std::string componentName) {
    if (GAME->GetEntityManager().CountEntities(entityId) == 0) {
        spdlog::warn("Entity '{}' not found!", entityId);
        return;
    }
    AnimationComponent* animationComponent = dynamic_cast<AnimationComponent*>(GAME->GetEntityManager().GetEntity(entityId).GetComponent(componentName));
    if (animationComponent == nullptr) {
        spdlog::warn("Entity '{}' does not have an animation component '{}'!", entityId, componentName);
        return;
    }
    animationComponent->Play();
}

void AnimationComponent::Start() {

}

void AnimationComponent::Update() {
    // can't do this in start, mesh renderer isn't initiated yet
    if (isPlaying_) {
        if (glfwGetTime() < animationStart_)
            return;
        float t = ((float) glfwGetTime() - animationStart_) / animationLength;
        if (t >= 1.0f) {
            t = 1.0f;
            isPlaying_ = false;
        }
        if (playReverse)
            t = 1.0f - t;
        Interpolate(t);
    }
}

void AnimationComponent::Play() {
    if (!allowInterruptions && isPlaying_)
        return;
    isPlaying_ = true;
    animationStart_ = (float) glfwGetTime() + animationDelay;
}

void MeshTransformAnimation::UpdateTransforms(const glm::mat4& transform) {
    for (int meshIndex : meshTransforms) {
        meshRenderer_->meshes.at(meshIndex)->transformMatrix = transform;
    }
}

void MeshTransformAnimation::Update() {
    if (meshRenderer_ == nullptr) {
        meshRenderer_ = parent->GetComponent<MeshRenderer>();
        if (transformAtStart) {
            glm::mat4 baseTransform = meshRenderer_->meshes.at(meshTransforms[0])->transformMatrix;
            baseTransform_ = baseTransform;
            UpdateTransforms(Transform(0));
        }
    }
    AnimationComponent::Update();
}

void MeshTransformAnimation::Interpolate(float t) {
    UpdateTransforms(Transform(t));
}

void MeshTransformAnimation::Play() {
    if (!transformAtStart)
        baseTransform_ = parent->GetComponent<MeshRenderer>()->meshes.at(meshTransforms[0])->transformMatrix;
    AnimationComponent::Play();
}


glm::mat4 MeshRotationAnimation::Transform(float t) {
    return glm::rotate(baseTransform_, glm::radians(t * angle * 2.0f - angle), rotationAxis);
}

void FloatAnimation::Interpolate(float t) {
    if (ptr != nullptr)
        *ptr = t;
}
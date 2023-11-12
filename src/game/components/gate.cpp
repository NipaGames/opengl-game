#include "gate.h"

void Gate::Interpolate(float t) {
    parent->transform->position.y = startHeight_ + t * height;
}

void Gate::Play() {
    startHeight_ = parent->transform->position.y;
    AnimationComponent::Play();
}
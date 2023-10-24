#include "gate.h"

void Gate::OpenGate(std::string gateId) {
    if (GAME->GetEntityManager().CountEntities(gateId) == 0) {
        spdlog::warn("Entity '{}' not found!", gateId);
        return;
    }
    Gate* gate = GAME->GetEntityManager().GetEntity(gateId).GetComponent<Gate>();
    if (gate == nullptr) {
        spdlog::warn("Entity '{}' does not have a Gate-component!", gateId);
        return;
    }
    gate->Open();
}

void Gate::Update() {
    if (opening_) {
        double x = glfwGetTime() - openingStart_;
        double height = startHeight_ + (x / openingSeconds) * openingHeight;
        if (x >= openingSeconds) {
            height = startHeight_ + openingHeight;
            opening_ = false;
            isOpened = true;
        }
        parent->transform->position.y = (float) height;
    }
}

void Gate::Open() {
    if (isOpened || opening_)
        return;
    opening_ = true;
    startHeight_ = parent->transform->position.y;
    openingStart_ = glfwGetTime();
}
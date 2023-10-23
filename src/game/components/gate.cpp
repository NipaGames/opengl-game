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
    gate->parent->Destroy();
}
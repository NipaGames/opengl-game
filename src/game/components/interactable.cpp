#include "interactable.h"
#include <game/event/eventparser.h>
#include <game/game.h>

#include <core/entity/entity.h>
#include <core/game.h>
#include <core/input.h>

Interactable::~Interactable() {
    if (previouslyInProximity_) {
        MonkeyGame::GetGame()->hud.HideInteractMessage();
    }
}

void Interactable::Start() {
    playerTransform_ = GAME->GetEntityManager().GetEntity("Player").transform;
}

void Interactable::Trigger() {
    auto e = [&] () {
        event.GetParser()->SetKeyword("this", parent->id);
        event.Trigger();
    };
    if (requestNextUpdate) {
        MonkeyGame::GetGame()->RequestEventNextUpdate(e);
    }
    else {
        e();
    }
}

void Interactable::Update() {
    if (oneUse && hasBeenUsed_)
        return;
    inProximity_ = glm::distance(parent->transform->position, playerTransform_->position) < range;
    if (inProximity_) {
        if (trigger == TriggerType::INTERACT && Input::IsKeyPressedDown(keyCode)) {
            Trigger();
            hasBeenUsed_ = true;
            if (oneUse)
                MonkeyGame::GetGame()->hud.HideInteractMessage();
        }
        if (!previouslyInProximity_) {
            if (trigger == TriggerType::IN_PROXIMITY) {
                Trigger();
                hasBeenUsed_ = true;
            }
            else if (trigger == TriggerType::INTERACT) {
                MonkeyGame::GetGame()->hud.ShowInteractText(message, keyCode);
            }
            previouslyInProximity_ = true;
        }
        if (trigger == TriggerType::IN_PROXIMITY_UPDATE) {
            Trigger();
            hasBeenUsed_ = true;
        }
    }
    else {
        if (previouslyInProximity_) {
            MonkeyGame::GetGame()->hud.HideInteractMessage();
        }
        previouslyInProximity_ = false;
    }
}

// serialization, will have to write a general enum version
#include <core/io/serializer.h>
#include <core/io/serializetypes.h>

JSON_SERIALIZE_ENUM(TriggerType);
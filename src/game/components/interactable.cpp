#include "interactable.h"
#include "../eventparser.h"

#include <core/entity/entity.h>
#include <core/game.h>
#include <core/input.h>

void Interactable::Start() {
    playerTransform_ = GAME->GetEntityManager().GetEntity("Player").transform;
}

void Interactable::Trigger() {
    event.GetParser()->SetKeyword("this", parent->id);
    event.Trigger();
}

void Interactable::Update() {
    inProximity_ = glm::distance(parent->transform->position, playerTransform_->position) < range;
    if (inProximity_) {
        if (trigger == TriggerType::INTERACT && Input::IsKeyPressedDown(GLFW_KEY_E)) {
            Trigger();
        }
        if (!previouslyInProximity_) {
            if (trigger == TriggerType::IN_PROXIMITY) {
                Trigger();
            }
            previouslyInProximity_ = true;
        }
    }
    else {
        previouslyInProximity_ = false;
    }
}

// serialization, will have to write a general enum version
#include <core/io/serializer.h>
#include <core/io/serializetypes.h>

JSON_SERIALIZE_TYPES([](Serializer::SerializationArgs& args, const nlohmann::json& j) {
    if (!j.is_string())
        return false;
    auto trigger = magic_enum::enum_cast<TriggerType>((std::string) j);
    if (!trigger.has_value())
        return false;
    args.Return(trigger.value());
    return true;
}, TriggerType);
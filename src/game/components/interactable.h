#pragma once

#include <core/entity/component.h>
#include <core/entity/transform.h>
#include <game/event.h>

enum class TriggerType {
    IN_PROXIMITY,
    INTERACT
};

class Interactable : public Component<Interactable> {
private:
    bool inProximity_ = false;
    bool previouslyInProximity_ = false;
    Transform* playerTransform_;
public:
    DEFINE_COMPONENT_DATA_VALUE(float, range, 2.5f);
    DEFINE_COMPONENT_DATA_VALUE(TriggerType, trigger, TriggerType::INTERACT);
    DEFINE_COMPONENT_DATA_VALUE_DEFAULT(Event, event);

    void Start();
    void Update();
    void Trigger();
};
#pragma once

#include <core/entity/component.h>
#include <core/entity/transform.h>
#include <game/event/event.h>

enum class TriggerType {
    IN_PROXIMITY,
    IN_PROXIMITY_UPDATE,
    INTERACT
};

class Interactable : public Component<Interactable> {
private:
    bool inProximity_ = false;
    bool hasBeenUsed_ = false;
    bool previouslyInProximity_ = false;
    Transform* playerTransform_;
public:
    DEFINE_COMPONENT_DATA_VALUE(float, range, 2.5f);
    DEFINE_COMPONENT_DATA_VALUE(int, keyCode, GLFW_KEY_E);
    DEFINE_COMPONENT_DATA_VALUE(TriggerType, trigger, TriggerType::INTERACT);
    DEFINE_COMPONENT_DATA_VALUE(bool, oneUse, false);
    DEFINE_COMPONENT_DATA_VALUE(bool, requestNextUpdate, false);
    DEFINE_COMPONENT_DATA_VALUE(std::string, message, "INTERACT");
    DEFINE_COMPONENT_DATA_VALUE_DEFAULT(Event, event);

    virtual ~Interactable();
    void Start();
    void Update();
    void Trigger();
};
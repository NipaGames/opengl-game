#pragma once

#include <core/entity/component.h>
#include <game/event.h>

class Interactable : public Component<Interactable> {
public:
    DEFINE_COMPONENT_DATA_VALUE(float, range, 1.0f);
    DEFINE_COMPONENT_DATA_VALUE_DEFAULT(Event, event);
};
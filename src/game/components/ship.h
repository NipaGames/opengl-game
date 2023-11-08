#pragma once

#include <core/entity/entitymanager.h>
#include <core/entity/component.h>

class Ship : public Component<Ship> {
public:
    DEFINE_COMPONENT_DATA_VALUE(float, speed, .5f);
    DEFINE_COMPONENT_DATA_VALUE(float, height, .5f);
    void FixedUpdate();
};

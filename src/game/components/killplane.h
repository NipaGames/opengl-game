#pragma once

#include <core/entity/component.h>

class VerticalKillPlane : public Component<VerticalKillPlane> {
public:
    DEFINE_COMPONENT_DATA_VALUE(bool, isLava, false);
    void FixedUpdate();
};

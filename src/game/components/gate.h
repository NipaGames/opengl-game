#pragma once

#include <core/entity/entitymanager.h>
#include <core/entity/component.h>
#include <core/game.h>
#include "animationcomponent.h"

class Gate : public AnimationComponent {
private:
    float startHeight_;
public:
    DEFINE_COMPONENT_DATA_VALUE(float, height, 4.5f);
    void Interpolate(float) override;
    void Play();
    void Start();
};
REGISTER_COMPONENT(Gate);
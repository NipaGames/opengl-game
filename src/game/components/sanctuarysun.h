#pragma once

#include <core/entity/entitymanager.h>
#include <core/entity/component.h>
#include <core/graphics/component/light.h>
#include <core/graphics/material.h>

class SanctuarySun : public Component<SanctuarySun> {
private:
    Lights::DirectionalLight* light_;
    float startTime_;
    float dayNightCycleLength_ = 60.0f;
    std::shared_ptr<Material> mountainMaterial_ = nullptr;
public:
    void Start();
    void FirstUpdate();
    void FixedUpdate();
};

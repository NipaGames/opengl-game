#pragma once

#include <core/entity/entitymanager.h>
#include <core/entity/component.h>
#include <core/game.h>

class Gate : public Component<Gate> {
private:
    bool opening_ = false;
    double openingStart_;
    float startHeight_;
public:
    DEFINE_COMPONENT_DATA_VALUE(double, openingSeconds, 2.0);
    DEFINE_COMPONENT_DATA_VALUE(float, openingHeight, 4.5f);
    bool isOpened = false;
    static void OpenGate(std::string);
    void Open();
    void Update();
};
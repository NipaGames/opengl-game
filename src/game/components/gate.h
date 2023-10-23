#pragma once

#include <core/entity/entitymanager.h>
#include <core/entity/component.h>
#include <core/game.h>

class Gate : public Component<Gate> {
public:
    static void OpenGate(std::string);
};
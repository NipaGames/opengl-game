#pragma once

#include "textcomponent.h"

namespace UI {
    class ButtonComponent : public TextComponent {
    private:
        bool prevHovering_ = false;
    public:
        bool isActive = true;
        void Update();
    };
    REGISTER_COMPONENT(ButtonComponent);
};
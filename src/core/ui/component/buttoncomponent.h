#pragma once

#include "textcomponent.h"

namespace UI {
    class ButtonComponent : public TextComponent {
    using TextComponent::TextComponent;
    private:
        bool prevHovering_ = false;
    public:
        void Update();
    };
    REGISTER_COMPONENT(ButtonComponent);
};
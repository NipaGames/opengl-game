#pragma once

#include <opengl.h>
#include "core/entity/component.h"
#include "core/entity/component/meshrenderer.h"
#include "core/ui/text.h"
#include "core/ui/uicomponent.h"

namespace UI {
    class TextComponent : public UIComponent<TextComponent> {
    private:
        Shader shader_;
    public:
        UI::Text::Font font;
        
        void Start();
        void Render(const glm::mat4&);
    };
};
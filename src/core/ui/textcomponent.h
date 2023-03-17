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
        std::string text_ = "";
    public:
        UI::Text::FontID font;
        glm::vec2 pos = glm::vec3(0.0f);
        glm::vec3 color = glm::vec3(1.0f);
        
        void Start();
        void Render(const glm::mat4&);
        void SetText(const std::string&);
        const std::string& GetText();
    };
};
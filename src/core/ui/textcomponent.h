#pragma once

#include <opengl.h>
#include "core/entity/component.h"
#include "core/entity/component/meshrenderer.h"
#include "core/ui/text.h"
#include "core/ui/uicomponent.h"

namespace UI {
    enum class TextRenderingMethod {
        RENDER_EVERY_FRAME,
        RENDER_TO_TEXTURE
    };

    class TextComponent : public UIComponent<TextComponent> {
    private:
        Shader shader_;
        std::string text_ = "";
        GLuint fbo_;
        GLuint texture_;
        GLuint charVao_, charVbo_;
        glm::ivec2 textSize_ = glm::vec2(0.0f);
        glm::ivec2 padding_ = glm::vec2(0.0f);
        TextRenderingMethod renderingMethod_;
        bool hasStarted_ = false;
    public:
        UI::Text::FontID font;
        glm::vec2 pos = glm::vec3(0.0f);
        glm::vec3 color = glm::vec3(1.0f);
        TextRenderingMethod renderingMethod = TextRenderingMethod::RENDER_TO_TEXTURE; // Must be declared before start, no effect otherwise
        
        virtual ~TextComponent();

        void Start();
        void Render(const glm::mat4&);
        void SetText(const std::string&);
        const std::string& GetText();
    };
};
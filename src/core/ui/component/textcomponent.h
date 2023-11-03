#pragma once

#include <opengl.h>

#include "uicomponent.h"
#include "../text.h"
#include "../canvas.h"
#include <core/entity/component.h>
#include <core/graphics/component/meshrenderer.h>
#include <core/graphics/shape.h>

namespace UI {
    enum class TextRenderingMethod {
        RENDER_EVERY_FRAME,
        RENDER_TO_TEXTURE
    };

    class TextComponent : public UIComponent {
    protected:
        Shader shader_;
        std::string text_ = "";
        GLuint fbo_ = NULL;
        GLuint texture_ = NULL;
        glm::ivec2 textSize_ = glm::vec2(0.0f);
        glm::ivec2 actualTextSize_ = glm::vec2(0.0f);
        glm::ivec2 padding_ = glm::vec2(0.0f);
        int additionalRows_ = 0;
        float additionalRowsHeight_ = 0;
        TextRenderingMethod renderingMethod_;
        bool hasStarted_ = false;
        Shape shape_;
        void ResizeText();
    public:
        std::string font;
        float lineSpacing = 5.0f;
        glm::vec4 color = glm::vec4(1.0f);
        TextRenderingMethod renderingMethod = TextRenderingMethod::RENDER_TO_TEXTURE; // Must be declared before start, no effect otherwise
        Text::TextAlignment alignment = Text::TextAlignment::LEFT;
        float textureResolutionModifier = 1.0f;
        bool isVisible = true;
        
        virtual ~TextComponent();
        TextComponent() = default;
        TextComponent(UI::Canvas* c, int p = 0) : UIComponent(c, p) { }
        
        void Start();
        void Render(const glm::mat4&);
        void UpdateWindowSize();
        void SetText(const std::string&);
        const std::string& GetText() const;
        const glm::ivec2& GetTextSize() const;
    };
    REGISTER_COMPONENT(TextComponent);
};
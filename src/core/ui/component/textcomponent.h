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
        Shader shader_ = Shader(Shaders::ShaderID::UI_TEXT);
        Shader textureShader_ = Shader(Shaders::ShaderID::UI_TEXT);
        std::string text_ = "";
        GLuint fbo_ = NULL;
        GLuint texture_ = NULL;
        glm::vec2 textSize_ = glm::vec2(0.0f);
        glm::vec2 actualTextSize_ = glm::vec2(0.0f);
        glm::vec2 padding_ = glm::vec2(0.0f);
        int additionalRows_ = 0;
        float additionalRowsHeight_ = 0;
        TextRenderingMethod renderingMethod_;
        bool hasStarted_ = false;
        Shape shape_;
        UI::Rect bounds_;
        void RenderTexture();
    public:
        std::string font;
        float lineSpacing = 5.0f;
        glm::vec4 color = glm::vec4(1.0f);
        TextRenderingMethod renderingMethod = TextRenderingMethod::RENDER_TO_TEXTURE; // Must be declared before start, no effect otherwise
        Text::TextAlignment alignment = Text::TextAlignment::LEFT;
        float textureResolutionModifier = 1.0f;
        
        virtual ~TextComponent();
        TextComponent() = default;
        TextComponent(UI::Canvas* c, int p = 0) : UIComponent(c, p) { }
        
        void Start();
        void Render(const glm::mat4&);
        void UpdateWindowSize();
        void SetText(const std::string&);
        void SetShader(const Shader&);
        const std::string& GetText() const;
        const glm::vec2& GetTextSize() const;
        const UI::Rect& GetBounds() const;
    };
    REGISTER_COMPONENT(TextComponent);
};
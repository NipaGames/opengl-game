#pragma once

#include <opengl.h>
#include "core/entity/component.h"
#include "core/entity/component/meshrenderer.h"
#include "core/graphics/shape.h"
#include "core/ui/text.h"
#include "core/ui/uicomponent.h"
#include "core/ui/canvas.h"

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
    glm::ivec2 padding_ = glm::vec2(0.0f);
    TextRenderingMethod renderingMethod_;
    bool hasStarted_ = false;
    Shape shape_;
    void ResizeText();
public:
    Text::FontID font;
    glm::vec4 color = glm::vec4(1.0f);
    TextRenderingMethod renderingMethod = TextRenderingMethod::RENDER_TO_TEXTURE; // Must be declared before start, no effect otherwise
    
    virtual ~TextComponent();
    TextComponent(UI::Canvas* c, int p = 0) : UIComponent(c, p) { }
    virtual IComponent* Clone() const override {
        TextComponent* c = new TextComponent(canvas_, priority_);
        if (this->isAddedToCanvas_)
            c->AddToCanvas();
        c->shader_ = this->shader_;
        c->text_ = this->text_;
        c->textSize_ = this->textSize_;
        c->padding_ = this->padding_;
        c->renderingMethod_ = this->renderingMethod_;
        c->font = this->font;
        c->color = this->color;
        c->renderingMethod = this->renderingMethod;
        c->hasStarted_ = this->hasStarted_;

        c->shape_ = Shape();
        if (hasStarted_ && renderingMethod_ == TextRenderingMethod::RENDER_TO_TEXTURE) {
            c->shape_.GenerateVAO();
            glGenBuffers(1, &c->fbo_);
            glGenTextures(1, &c->texture_);
            c->SetText(c->text_);
        }
        return c;
    }
    
    void Start();
    void Render(const glm::mat4&);
    void UpdateWindowSize();
    void SetText(const std::string&);
    const std::string& GetText();
};
};
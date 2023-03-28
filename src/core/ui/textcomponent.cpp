#include "core/ui/textcomponent.h"

#include <algorithm>
#include "core/game.h"

UI::TextComponent::~TextComponent() {
    if (renderingMethod_ == TextRenderingMethod::RENDER_TO_TEXTURE) {
        glDeleteBuffers(1, &fbo_);
        glDeleteTextures(1, &texture_);
    }
}

void UI::TextComponent::Start() {
    shader_ = Shader(SHADER_UI_TEXT);
    renderingMethod_ = renderingMethod;
    glGenVertexArrays(1, &charVao_);
    glGenBuffers(1, &charVbo_);
    glBindVertexArray(charVao_);
    glBindBuffer(GL_ARRAY_BUFFER, charVbo_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    hasStarted_ = true;
    if (renderingMethod_ == TextRenderingMethod::RENDER_TO_TEXTURE) {
        glGenFramebuffers(1, &fbo_);
        glGenTextures(1, &texture_);
        SetText(text_);
    }
    game->GetRenderer().AddUIComponent(this);
}

void UI::TextComponent::Render(const glm::mat4& projection) {
    shader_.Use();
    shader_.SetUniform("textColor", color);
    shader_.SetUniform("projection", projection);
    glm::vec2 pos(parent->transform->position.x, parent->transform->position.y);
    // janky ass way to determine the size
    glm::vec3& sizeVec = parent->transform->size;
    float size = std::max({ sizeVec.x, sizeVec.y, sizeVec.z });

    if (renderingMethod_ == TextRenderingMethod::RENDER_EVERY_FRAME) {
        UI::Text::RenderText(UI::Text::GetFont(font), text_, pos, size);
    }
    else if (renderingMethod_ == TextRenderingMethod::RENDER_TO_TEXTURE) {
        float w = textSize_.x * size;
        float h = textSize_.y * size;
        glActiveTexture(GL_TEXTURE0);
        glBindVertexArray(charVao_);
        float vertices[6][4] = {
            { pos.x,     pos.y - padding_[0] + h,   0.0f, 1.0f },            
            { pos.x,     pos.y - padding_[0],       0.0f, 0.0f },
            { pos.x + w, pos.y - padding_[0],       1.0f, 0.0f },

            { pos.x,     pos.y - padding_[0] + h,   0.0f, 1.0f },
            { pos.x + w, pos.y - padding_[0],       1.0f, 0.0f },
            { pos.x + w, pos.y - padding_[0] + h,   1.0f, 1.0f }   
        };

        glBindTexture(GL_TEXTURE_2D, texture_);
        glBindBuffer(GL_ARRAY_BUFFER, charVbo_);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); 
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}

void UI::TextComponent::SetText(const std::string& t) {
    text_ = t;
    if (renderingMethod_ == TextRenderingMethod::RENDER_TO_TEXTURE && hasStarted_) {
        auto f = UI::Text::GetFont(font);
        padding_ = UI::Text::GetVerticalPadding(f, text_);
        textSize_ = glm::ivec2(UI::Text::GetTextWidth(f, text_), padding_[1] + padding_[0]);

        glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
        glBindTexture(GL_TEXTURE_2D, texture_);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textSize_.x, textSize_.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glClearTexImage(texture_, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texture_, 0);
        glDrawBuffer(GL_COLOR_ATTACHMENT0);

        // finally drawing
        shader_.Use();
        shader_.SetUniform("textColor", glm::vec3(1.0f));
        shader_.SetUniform("projection", glm::ortho(0.0f, 1280.0f, 0.0f, 720.0f));
        UI::Text::RenderText(f, text_, glm::vec2(0, padding_[0]), 1.0f);
    }
}

const std::string& UI::TextComponent::GetText() {
    return text_;
}
#include "textcomponent.h"

#include <algorithm>
#include <core/game.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>

UI::TextComponent::~TextComponent() {
    if (renderingMethod_ == TextRenderingMethod::RENDER_TO_TEXTURE) {
        glDeleteBuffers(1, &fbo_);
        glDeleteTextures(1, &texture_);
    }
}

void UI::TextComponent::Start() {
    shader_ = Shader(Shaders::ShaderID::UI_TEXT);
    renderingMethod_ = renderingMethod;
    shape_.numVertexAttributes = 4;
    shape_.stride = 4;
    shape_.GenerateVAO();

    hasStarted_ = true;
    if (renderingMethod_ == TextRenderingMethod::RENDER_TO_TEXTURE) {
        glGenFramebuffers(1, &fbo_);
        glGenTextures(1, &texture_);
        SetText(text_);
    }
}

void UI::TextComponent::Render(const glm::mat4& projection) {
    if (!isVisible)
        return;
    shader_.Use();
    shader_.SetUniform("textColor", color);
    shader_.SetUniform("projection", projection);
    glm::vec2 pos(parent->transform->position.x, parent->transform->position.y);
    // janky ass way to determine the size
    float size = parent->transform->size.z;

    auto& f = GAME->resources.fontManager.Get(font);
    glm::ivec2 windowSize;
    glfwGetWindowSize(GAME->GetGameWindow().GetWindow(), &windowSize.x, &windowSize.y);
    float modifier = (16.0f * windowSize.y) / (9.0f * windowSize.x);
    if (renderingMethod_ == TextRenderingMethod::RENDER_EVERY_FRAME) {
        if (alignment == Text::TextAlignment::RIGHT) {
            pos.x -= Text::GetTextWidth(f, text_) * size;
        }
        UI::Text::RenderText(GAME->resources.fontManager.Get(font), text_, pos, size, modifier, alignment, lineSpacing * size);
    }
    else if (renderingMethod_ == TextRenderingMethod::RENDER_TO_TEXTURE) {
        float w = textSize_.x * size * modifier * (BASE_FONT_SIZE / (float) f.size.y);
        float h = textSize_.y * size;
        float marginY = (additionalRowsHeight_ + padding_[0]) * size;
        actualTextSize_ = { w, h };
        glActiveTexture(GL_TEXTURE0);

        switch (alignment) {
            case Text::TextAlignment::LEFT:
                shape_.SetVertexData(new float[24] {
                    pos.x,     pos.y - marginY + h,   0.0f, 1.0f,
                    pos.x,     pos.y - marginY,       0.0f, 0.0f,
                    pos.x + w, pos.y - marginY,       1.0f, 0.0f,

                    pos.x,     pos.y - marginY + h,   0.0f, 1.0f,
                    pos.x + w, pos.y - marginY,       1.0f, 0.0f,
                    pos.x + w, pos.y - marginY + h,   1.0f, 1.0f
                }, false);
                break;
            case Text::TextAlignment::RIGHT:
                shape_.SetVertexData(new float[24] {
                    pos.x - w,     pos.y - marginY + h,   0.0f, 1.0f,
                    pos.x - w,     pos.y - marginY,       0.0f, 0.0f,
                    pos.x, pos.y - marginY,       1.0f, 0.0f,

                    pos.x - w,     pos.y - marginY + h,   0.0f, 1.0f,
                    pos.x, pos.y - marginY,       1.0f, 0.0f,
                    pos.x, pos.y - marginY + h,   1.0f, 1.0f
                }, false);
                break;
        }
        shape_.Bind();

        glBindTexture(GL_TEXTURE_2D, texture_);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}

void UI::TextComponent::ResizeText() {
    auto& f = GAME->resources.fontManager.Get(font);
    padding_ = UI::Text::GetVerticalPadding(f, text_);

    glm::ivec2 windowSize;
    glfwGetWindowSize(GAME->GetGameWindow().GetWindow(), &windowSize.x, &windowSize.y);
    textSize_ = glm::vec2(UI::Text::GetTextWidth(f, text_), UI::Text::GetTextHeight(f, text_, (int) lineSpacing));
    glm::ivec2 texSize;
    float m = textureResolutionModifier;
    float fontModifier = f.size.y / (float) BASE_FONT_SIZE;
    bool renderWndSizeTextures = textSize_.x * m * fontModifier > windowSize.x || textSize_.y * m * fontModifier > windowSize.y;
    if (renderWndSizeTextures) {
        m *= parent->transform->size.z;
        texSize = (glm::vec2) windowSize * m;
    }
    else {
        texSize = (glm::vec2) textSize_ * m;
    }
    texSize.y = (int) ((float) texSize.y * fontModifier);

    glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
    glBindTexture(GL_TEXTURE_2D, texture_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texSize.x, texSize.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glClearTexImage(texture_, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texture_, 0);
    glDrawBuffer(GL_COLOR_ATTACHMENT0);
    
    // finally drawing
    shader_.Use();
    shader_.SetUniform("textColor", glm::vec4(1.0f));
    if (renderWndSizeTextures) {
        shader_.SetUniform("projection", glm::ortho(0.0f, (float) textSize_.x, 0.0f, (float) textSize_.y));
    }
    else {
        shader_.SetUniform("projection", glm::ortho(0.0f, (float) windowSize.x, 0.0f, (float) windowSize.y));
    }
    UI::Text::RenderText(f, text_, glm::vec2(0, additionalRowsHeight_ + padding_[0] * m) * m * fontModifier, m, 1.0f, alignment, lineSpacing * m);
}

void UI::TextComponent::SetText(const std::string& t) {
    text_ = t;
    additionalRows_ = (int) std::count(text_.begin(), text_.end(), '\n');
    additionalRowsHeight_ = additionalRows_ * (GAME->resources.fontManager.Get(font).fontHeight + lineSpacing);
    if (renderingMethod_ == TextRenderingMethod::RENDER_TO_TEXTURE && hasStarted_) {
        ResizeText();
    }
}

const std::string& UI::TextComponent::GetText() const {
    return text_;
}

const glm::ivec2& UI::TextComponent::GetTextSize() const {
    return actualTextSize_;
}

void UI::TextComponent::UpdateWindowSize() {
    if (renderingMethod_ == TextRenderingMethod::RENDER_TO_TEXTURE && hasStarted_) {
        ResizeText();
    }
}
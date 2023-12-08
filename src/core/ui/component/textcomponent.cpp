#include "textcomponent.h"

#include <algorithm>
#include <core/game.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>

UI::TextComponent::~TextComponent() {
    if (renderingMethod_ == TextRenderingMethod::RENDER_TO_TEXTURE) {
        if (fbo_ != NULL)
            glDeleteFramebuffers(1, &fbo_);
        if (texture_ != NULL)
            glDeleteTextures(1, &texture_);
    }
    fbo_ = NULL;
    texture_ = NULL;
}

void UI::TextComponent::Start() {
    renderingMethod_ = renderingMethod;
    shape_.numVertexAttributes = 4;
    shape_.stride = 4;
    shape_.GenerateVAO();

    hasStarted_ = true;
    if (renderingMethod_ == TextRenderingMethod::RENDER_TO_TEXTURE) {
        glGenFramebuffers(1, &fbo_);
        glGenTextures(1, &texture_);
        RenderTexture();
    }
}

void UI::TextComponent::SetShader(const Shader& s) {
    shader_ = s;
    if (renderingMethod_ == TextRenderingMethod::RENDER_TO_TEXTURE && hasStarted_)
        RenderTexture();
}

void UI::TextComponent::Render(const glm::mat4& projection) {
    if (!isVisible || color.w == 0.0f)
        return;
    shader_.Use();
    shader_.SetUniform("textColor", color);
    shader_.SetUniform("projection", projection);
    shader_.SetUniform("time", (float) glfwGetTime());
    glm::vec2 pos(parent->transform->position.x, parent->transform->position.y);
    // janky ass way to determine the size
    float size = parent->transform->size.z;

    auto& f = GAME->resources.fontManager.Get(font);
    glm::ivec2 windowSize;
    glfwGetWindowSize(GAME->GetGameWindow().GetWindow(), &windowSize.x, &windowSize.y);
    float modifier = (16.0f * windowSize.y) / (9.0f * windowSize.x);
    if (renderingMethod_ == TextRenderingMethod::RENDER_EVERY_FRAME) {
        switch (alignment) {
            case Text::TextAlignment::RIGHT:
                pos.x -= Text::GetTextWidth(f, text_) * size;
                break;
            case Text::TextAlignment::CENTER:
                pos.x -= (Text::GetTextWidth(f, text_) * size) / 2.0f;
                break;
        }
        UI::Text::RenderText(GAME->resources.fontManager.Get(font), text_, pos, size * ((float) BASE_FONT_SIZE / f.size.y), modifier, alignment, lineSpacing * size);
    }
    else if (renderingMethod_ == TextRenderingMethod::RENDER_TO_TEXTURE) {
        glm::vec2 wndRatio = (glm::vec2) windowSize / glm::vec2(1280.0f, 720.0f);
        int w = (int) (textSize_.x * modifier);
        int h = (int) (textSize_.y);
        float marginY = (additionalRowsHeight_ + padding_[0] * ((float) BASE_FONT_SIZE / f.size.y)) * size;
        glActiveTexture(GL_TEXTURE0);

        glm::vec2 offset(0.0f, -marginY);
        switch (alignment) {
            case Text::TextAlignment::RIGHT:
                offset.x = (float) -w;
                break;
            case Text::TextAlignment::CENTER:
                offset.x = -w / 2.0f;
                break;
        }
        shape_.SetVertexData(new float[24] {
            pos.x + offset.x,     pos.y + offset.y + h,   0.0f, 1.0f,
            pos.x + offset.x,     pos.y + offset.y,       0.0f, 0.0f,
            pos.x + offset.x + w, pos.y + offset.y,       1.0f, 0.0f,

            pos.x + offset.x,     pos.y + offset.y + h,   0.0f, 1.0f,
            pos.x + offset.x + w, pos.y + offset.y,       1.0f, 0.0f,
            pos.x + offset.x + w, pos.y + offset.y + h,   1.0f, 1.0f
        }, false);
        shape_.Bind();

        glBindTexture(GL_TEXTURE_2D, texture_);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}

void UI::TextComponent::RenderTexture() {
    auto& f = GAME->resources.fontManager.Get(font);
    padding_ = UI::Text::GetVerticalPadding(f, text_);

    glm::ivec2 windowSize;
    glfwGetWindowSize(GAME->GetGameWindow().GetWindow(), &windowSize.x, &windowSize.y);
    float fontModifier = ((float) BASE_FONT_SIZE / f.size.y);
    float size = parent->transform->size.z;
    glm::vec2 wndRatio = (glm::vec2) windowSize / glm::vec2(1280.0f, 720.0f);
    textSize_ = glm::vec2(UI::Text::GetTextWidth(f, text_), UI::Text::GetTextHeight(f, text_, (int) lineSpacing) + padding_[0] * fontModifier) * size;
    glm::ivec2 texSize = (glm::ivec2) (wndRatio * (glm::vec2) textSize_);

    glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
    glBindTexture(GL_TEXTURE_2D, texture_);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    GLfloat borderColor[4] = { 0, 0, 0, 1 };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texSize.x, texSize.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glClearTexImage(texture_, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texture_, 0);
    glDrawBuffer(GL_COLOR_ATTACHMENT0);
    
    // finally drawing
    textureShader_.Use();
    textureShader_.SetUniform("textColor", glm::vec4(1.0f));
    textureShader_.SetUniform("projection", glm::ortho(0.0f, 1280.0f, 0.0f, 720.0f));

    UI::Text::RenderText(f, text_, glm::vec2(0, additionalRowsHeight_ * size + padding_[0] * size * fontModifier), size * fontModifier, 1.0f, alignment, lineSpacing * size);
    
    float modifier = (16.0f * windowSize.y) / (9.0f * windowSize.x);
    actualTextSize_ = { textSize_.x * modifier, textSize_.y };
}

void UI::TextComponent::SetText(const std::string& t) {
    if (text_ == t)
        return;
    text_ = t;
    additionalRows_ = (int) std::count(text_.begin(), text_.end(), '\n');
    additionalRowsHeight_ = additionalRows_ * (GAME->resources.fontManager.Get(font).fontHeight + lineSpacing);
    if (renderingMethod_ == TextRenderingMethod::RENDER_TO_TEXTURE && hasStarted_) {
        RenderTexture();
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
        RenderTexture();
    }
}
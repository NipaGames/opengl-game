#include "textcomponent.h"

#include <algorithm>
#include <core/game.h>

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
    shader_.Use();
    shader_.SetUniform("textColor", color);
    shader_.SetUniform("projection", projection);
    glm::vec2 pos(parent->transform->position.x, parent->transform->position.y + padding_[0] / 2);
    // janky ass way to determine the size
    float size = parent->transform->size.z;

    glm::ivec2 windowSize;
    glfwGetWindowSize(game->GetGameWindow().GetWindow(), &windowSize.x, &windowSize.y);
    float modifier = (16.0f * windowSize.y) / (9.0f * windowSize.x);

    if (renderingMethod_ == TextRenderingMethod::RENDER_EVERY_FRAME) {
        UI::Text::RenderText(game->resources.fontManager.Get(font), text_, pos, size, modifier);
    }
    else if (renderingMethod_ == TextRenderingMethod::RENDER_TO_TEXTURE) {
        float w = textSize_.x * size * modifier;
        float h = textSize_.y * size;
        actualTextSize_ = { w, h };
        glActiveTexture(GL_TEXTURE0);
        float vertices[] = {
            pos.x,     pos.y - padding_[0] + h,   0.0f, 1.0f,          
            pos.x,     pos.y - padding_[0],       0.0f, 0.0f,
            pos.x + w, pos.y - padding_[0],       1.0f, 0.0f,

            pos.x,     pos.y - padding_[0] + h,   0.0f, 1.0f,
            pos.x + w, pos.y - padding_[0],       1.0f, 0.0f,
            pos.x + w, pos.y - padding_[0] + h,   1.0f, 1.0f
        };
        shape_.SetVertexData(vertices);
        shape_.Bind();

        glBindTexture(GL_TEXTURE_2D, texture_);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}

void UI::TextComponent::ResizeText() {
    auto& f = game->resources.fontManager.Get(font);
    padding_ = UI::Text::GetVerticalPadding(f, text_);
    textSize_ = glm::ivec2(UI::Text::GetTextWidth(f, text_), padding_[1] + padding_[0]);

    glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
    glBindTexture(GL_TEXTURE_2D, texture_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textSize_.x, textSize_.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
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
    glm::ivec2 windowSize;
    glfwGetWindowSize(game->GetGameWindow().GetWindow(), &windowSize.x, &windowSize.y);
    shader_.SetUniform("projection", glm::ortho(0.0f, (float) windowSize.x, 0.0f, (float) windowSize.y));
    UI::Text::RenderText(f, text_, glm::vec2(0, padding_[0]), 1.0f, 1.0f);
}

void UI::TextComponent::SetText(const std::string& t) {
    text_ = t;
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
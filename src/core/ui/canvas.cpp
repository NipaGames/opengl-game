#include "canvas.h"
#include "component/uicomponent.h"
#include <core/game.h>

UI::Canvas::Canvas() : bgShader_(Shaders::ShaderID::UI_SHAPE) {
    bgShape_.numVertexAttributes = 2;
    bgShape_.GenerateVAO();
}

UI::Canvas::~Canvas() {
    for (auto it : components_)
        it.second->canvas_ = nullptr;
}

void UI::Canvas::Draw() {
    if (!isVisible)
        return;
    auto proj = GetProjectionMatrix();
    if (bgColor != glm::vec4(0.0f)) {
        bgShader_.Use();
        bgShader_.SetUniform("projection", proj);
        bgShader_.SetUniform("shapeColor", bgColor);
        float w = bgSize.x;
        float h = bgSize.y;
        float vertices[] = {
            0,  0,
            0, -h,
            w, -h,
            0,  0,
            w, -h,
            w,  0
        };
        bgShape_.SetVertexData(vertices);
        bgShape_.Bind();
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    for (auto it : components_) {
        it.second->Render(proj);
    }
}

glm::mat4 UI::Canvas::GetProjectionMatrix() const {
    glm::mat4 proj = glm::ortho(
        (float) offset.x,
        (float) 1280 + offset.x,
        (float) -offset.y,
        (float) 720 - offset.y);
    return proj;
}

void UI::Canvas::AddUIComponent(UI::UIComponent* c, int priority) {
    components_.insert({ priority, c });
    c->canvas_ = this;
}

void UI::Canvas::UpdateWindowSize() {
    for (auto it : components_) {
        it.second->UpdateWindowSize();
    }
}

void UI::Canvas::RemoveUIComponent(const UI::UIComponent* c) {
    for (auto it = components_.begin(); it != components_.end(); it++) {
        if (it->second == c) {
            components_.erase(it);
            return;
        }
    }
}
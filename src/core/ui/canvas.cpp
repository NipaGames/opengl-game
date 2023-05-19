#include "canvas.h"
#include "component/uicomponent.h"
#include <core/game.h>

UI::Canvas::Canvas() {
    offset = glm::ivec2(0, 0);
    bgShader_ = Shader(SHADER_UI_SHAPE);
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
        bgShape_.Bind();
        float w = bgSize.x;
        float h = bgSize.y;
        float vertices[6][4] = {
            { 0,  0,  0.0f, 1.0f },            
            { 0, -h,  0.0f, 0.0f },
            { w, -h,  1.0f, 0.0f },
            { 0,  0,  0.0f, 1.0f },
            { w, -h,  1.0f, 0.0f },
            { w,  0,  1.0f, 1.0f }   
        };
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    for (auto it : components_) {
        it.second->Render(proj);
    }
}

glm::mat4 UI::Canvas::GetProjectionMatrix() const {
    glm::mat4 proj = glm::ortho((float) offset.x,
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

void UI::Canvas::RemoveUIComponent(UI::UIComponent* c) {
    for (auto it = components_.begin(); it != components_.end(); it++) {
        if (it->second == c) {
            components_.erase(it);
            return;
        }
    }
}
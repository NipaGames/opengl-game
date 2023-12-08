#include "canvas.h"
#include "component/uicomponent.h"
#include <core/game.h>

UI::Canvas::Canvas() : bgShader_(Shaders::ShaderID::UI_SHAPE) {

}

UI::Canvas::Canvas(Canvas&& c) : 
    bgShader_(c.bgShader_),
    bgShape_(c.bgShape_),
    isVisible(c.isVisible),
    offset(c.offset),
    bgMaterial(c.bgMaterial),
    bgSize(c.bgSize),
    components_(c.components_)
{
    for (auto it : components_) {
        it.second->canvas_ = this;
    }
    c.components_.clear();
}

UI::Canvas::~Canvas() {
    for (auto it : components_) {
        if (it.second->canvas_ == this)
            it.second->canvas_ = nullptr;
    }
}

UI::Canvas* UI::Canvas::GetCanvas() {
    return dynamic_cast<Canvas*>(this);
}

void UI::Canvas::GenerateBackgroundShape() {
    bgShape_.numVertexAttributes = 4;
    bgShape_.stride = 4;
    bgShape_.GenerateVAO();
}

void UI::Canvas::Draw() {
    if (!isVisible)
        return;
    auto proj = GetProjectionMatrix();
    if (bgMaterial != nullptr) {
        bgShader_.Use();
        bgShader_.SetUniform("projection", proj);
        bgMaterial->Use(bgShader_);
        bgMaterial->BindTexture();
        float w = bgSize.x;
        float h = bgSize.y;
        float vertices[] = {
            // pos      // texCoords
            0,  0,      0.0f, 0.0f,
            0, -h,      0.0f, 1.0f,
            w, -h,      1.0f, 1.0f,
            0,  0,      0.0f, 0.0f,
            w, -h,      1.0f, 1.0f,
            w,  0,      1.0f, 0.0f
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
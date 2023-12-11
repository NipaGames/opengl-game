#include "canvas.h"
#include "component/uicomponent.h"
#include <core/game.h>

UI::Canvas::Canvas() :
    bgShader_(Shaders::ShaderID::UI_SHAPE)
{ }

UI::Canvas::Canvas(Canvas&& c) : 
    bgShader_(c.bgShader_),
    bgShape_(c.bgShape_),
    bgMaterial(c.bgMaterial),
    isVisible(c.isVisible),
    offset(c.offset),
    bgSize(c.bgSize),
    bgVerticalAnchor(c.bgVerticalAnchor)
{
    components_ = std::move(c.components_);
    for (auto it : components_) {
        it.second->canvas_ = this;
    }
}

UI::Canvas& UI::Canvas::operator=(Canvas&& c) {
    bgShader_ = c.bgShader_;
    bgShape_ = c.bgShape_;
    bgMaterial = c.bgMaterial;
    isVisible = c.isVisible;
    offset = c.offset;
    bgSize = c.bgSize;
    bgVerticalAnchor = c.bgVerticalAnchor;
    components_ = std::move(c.components_);
    for (auto it : components_) {
        it.second->canvas_ = this;
    }
    return *this;
}

UI::Canvas::~Canvas() {
    for (auto it : components_) {
        if (it.second->canvas_ == this)
            it.second->canvas_ = nullptr;
    }
}

UI::Canvas* UI::Canvas::GetCanvas() {
    return static_cast<Canvas*>(this);
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
        float top = bgVerticalAnchor == CanvasBackgroundVerticalAnchor::OVER ? h : 0;
        float bottom = bgVerticalAnchor == CanvasBackgroundVerticalAnchor::OVER ? 0 : -h;
        float vertices[] = {
            // pos      // texCoords
            0, top,     0.0f, 0.0f,
            0, bottom,  0.0f, 1.0f,
            w, bottom,  1.0f, 1.0f,
            0, top,     0.0f, 0.0f,
            w, bottom,  1.0f, 1.0f,
            w, top,     1.0f, 0.0f
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
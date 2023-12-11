#include "canvas.h"
#include "component/uicomponent.h"
#include <core/game.h>

using namespace UI;

Canvas::Canvas() :
    bgShader_(Shaders::ShaderID::UI_SHAPE)
{ }

Canvas::Canvas(Canvas&& c) : 
    bgShader_(c.bgShader_),
    bgShape_(c.bgShape_),
    bgMaterial(c.bgMaterial),
    isVisible(c.isVisible),
    offset(c.offset),
    bgSize(c.bgSize),
    bgVerticalAnchor(c.bgVerticalAnchor)
{
    components_ = std::move(c.components_);
    for (const auto& [p, components] : components_) {
        for (UIComponent* c : components) {
            c->canvas_ = this;
        }
    }
}

Canvas& Canvas::operator=(Canvas&& c) {
    bgShader_ = c.bgShader_;
    bgShape_ = c.bgShape_;
    bgMaterial = c.bgMaterial;
    isVisible = c.isVisible;
    offset = c.offset;
    bgSize = c.bgSize;
    bgVerticalAnchor = c.bgVerticalAnchor;
    components_ = std::move(c.components_);
    for (const auto& [p, components] : components_) {
        for (UIComponent* c : components) {
            c->canvas_ = this;
        }
    }
    return *this;
}

Canvas::~Canvas() {
    for (const auto& [p, components] : components_) {
        for (UIComponent* c : components) {
            if (c->canvas_ == this)
                c->canvas_ = nullptr;
        }
    }
    components_.clear();
}

Canvas* Canvas::GetCanvas() {
    return static_cast<Canvas*>(this);
}

void Canvas::GenerateBackgroundShape() {
    bgShape_.numVertexAttributes = 4;
    bgShape_.stride = 4;
    bgShape_.GenerateVAO();
}

void Canvas::Draw() {
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

    for (const auto& [p, components] : components_) {
        for (UIComponent* c : components) {
            if (c->isVisible)
                c->Render(proj);
        }
    }
}

glm::mat4 Canvas::GetProjectionMatrix() const {
    glm::mat4 proj = glm::ortho(
        (float) offset.x,
        (float) 1280 + offset.x,
        (float) -offset.y,
        (float) 720 - offset.y);
    return proj;
}

void Canvas::AddUIComponent(UIComponent* c, int priority) {
    components_[priority].push_back(c);
    c->canvas_ = this;
}

void Canvas::UpdateWindowSize() {
    for (const auto& [p, components] : components_) {
        for (UIComponent* c : components) {
            c->UpdateWindowSize();
        }
    }
}

void Canvas::RemoveUIComponent(const UI::UIComponent* c) {
    for (auto& [p, components] : components_) {
        const auto it = std::find(components.begin(), components.end(), c);
        if (it == components.end())
            continue;
        components.erase(it);
        return;
    }
}
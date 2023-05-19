#include "canvas.h"
#include "component/uicomponent.h"
#include <core/game.h>

UI::Canvas::Canvas() {
    offset_ = glm::ivec2(0, 0);
}

UI::Canvas::~Canvas() {
    for (auto it : components_)
        it.second->canvas_ = nullptr;
}

void UI::Canvas::Draw() {
    if (!isVisible)
        return;
    auto proj = GetProjectionMatrix();
    for (auto it : components_) {
        it.second->Render(proj);
    }
}

void UI::Canvas::SetOffset(const glm::ivec2& o) {
    offset_ = o;
}

glm::mat4 UI::Canvas::GetProjectionMatrix() {
    glm::mat4 proj = glm::ortho((float) -offset_.x,
                                (float) 1280 + offset_.x,
                                (float) -offset_.y,
                                (float) 720 + offset_.y);
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
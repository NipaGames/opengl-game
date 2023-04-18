#include "core/ui/canvas.h"
#include "core/game.h"

UI::Canvas::Canvas() {
    offset_ = glm::ivec2(0, 0);
}

void UI::Canvas::Draw() {
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
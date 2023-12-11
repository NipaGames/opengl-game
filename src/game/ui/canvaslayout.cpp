#include "canvaslayout.h"
#include <game/game.h>

void CanvasLayout::Destroy() {
    for (UI::UIComponent* component : ownedComponents_) {
        delete component;
    }
    ownedComponents_.clear();
    pages_.clear();
}

CanvasLayout::~CanvasLayout() {
    Destroy();
}

CanvasLayout& CanvasLayout::operator=(CanvasLayout&& moveFrom) {
    Destroy();
    UI::Canvas::operator=(std::move(moveFrom));
    ownedComponents_ = std::move(moveFrom.ownedComponents_);
    pages_ = std::move(moveFrom.pages_);
    return *this;
}

void CanvasLayout::SetCurrentPage(const std::string& page) {
    // if we don't skip, the next update will register
    skipNextUpdate_ = true;
    for (const auto& [p, components] : pages_) {
        bool active = (p == page);
        for (UI::UIComponent* c : components) {
            c->isVisible = active;
            c->isActive = active;
        }
    }
}

void CanvasLayout::CloseCurrentPage() {
    for (const auto& [p, components] : pages_) {
        for (UI::UIComponent* c : components) {
            c->isVisible = false;
            c->isActive = false;
        }
    }
}

void CanvasLayout::Start() {
    AssignToRenderer(GAME->GetRenderer());
    for (UI::UIComponent* component : ownedComponents_) {
        component->IStart();
    }
}

void CanvasLayout::Update() {
    for (UI::UIComponent* component : ownedComponents_) {
        if (skipNextUpdate_)
            break;
        if (component->isActive)
            component->IUpdate();
    }
    skipNextUpdate_ = false;
}
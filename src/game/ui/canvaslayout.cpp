#include "canvaslayout.h"
#include <game/game.h>

void CanvasLayout::Destroy() {
    for (UI::UIComponent* component : ownedComponents_) {
        delete component;
    }
    ownedComponents_.clear();
}

CanvasLayout::~CanvasLayout() {
    Destroy();
}

CanvasLayout& CanvasLayout::operator=(CanvasLayout&& moveFrom) {
    Destroy();
    UI::Canvas::operator=(std::move(moveFrom));
    ownedComponents_ = std::move(moveFrom.ownedComponents_);
    return *this;
}

void CanvasLayout::Start() {
    AssignToRenderer(GAME->GetRenderer());
    for (UI::UIComponent* component : ownedComponents_) {
        component->IStart();
    }
}

void CanvasLayout::Update() {
    for (UI::UIComponent* component : ownedComponents_) {
        component->IUpdate();
    }
}
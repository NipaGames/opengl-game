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
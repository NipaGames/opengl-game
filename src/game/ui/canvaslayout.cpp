#include "canvaslayout.h"
#include <game/game.h>

CanvasLayout::~CanvasLayout() {
    for (UI::UIComponent* component : ownedComponents_) {
        delete component;
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
        component->IUpdate();
    }
}
#include "buttoncomponent.h"
#include <core/input.h>
#include <core/game.h>

using namespace UI;

void ButtonComponent::Update() {
    if (!isActive)
        return;
    const glm::vec2& mousePos = GAME->GetGameWindow().GetRelativeMousePosition();
    const UI::Rect& rect = GetBounds();
    if (mousePos.x > rect.left && mousePos.x < rect.right && mousePos.y > rect.bottom && mousePos.y < rect.top) {
        if (!prevHovering_) {
            eventHandler.Dispatch("mouseEnter");
        }
        prevHovering_ = true;
        if (Input::IsMouseButtonPressedDown(GLFW_MOUSE_BUTTON_1)) {
            eventHandler.Dispatch("click");
        }
    }
    else {
        if (prevHovering_) {
            eventHandler.Dispatch("mouseLeave");
        }
        prevHovering_ = false;
    }
}
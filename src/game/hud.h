#pragma once

#include <core/ui/canvas.h>
#include <core/ui/component/textcomponent.h>

class HUD {
public:
    std::string canvasId = "hud";
    std::string fontId;
    UI::TextComponent* interactMsg = nullptr;
    void CreateHUDElements();
    void ShowInteractMessage(const std::string&);
    void HideInteractMessage();
};
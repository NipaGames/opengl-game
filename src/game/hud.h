#pragma once

#include <core/ui/canvas.h>
#include <core/ui/component/textcomponent.h>

class HUD {
public:
    std::string canvasId = "hud";
    std::string fontId;
    UI::TextComponent* interactText = nullptr;
    UI::TextComponent* areaText = nullptr;
    void CreateHUDElements();
    void ShowInteractText(const std::string&, int = -1);
    void HideInteractMessage();
};
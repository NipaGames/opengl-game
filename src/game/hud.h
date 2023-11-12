#pragma once

#include <core/ui/canvas.h>
#include <core/ui/component/textcomponent.h>
#include "components/animationcomponent.h"

class HUD {
private:
    float fadeAreaTextAway_;
    bool isAreaTextShown_ = false;
    const float areaTextSeconds_ = 5.0f;
public:
    std::string canvasId = "hud";
    UI::TextComponent* interactText = nullptr;
    UI::TextComponent* areaText = nullptr;
    UI::TextComponent* hpText = nullptr;
    UI::TextComponent* maxHpText = nullptr;
    UI::TextComponent* statusText = nullptr;
    FloatAnimation* areaAnimation = nullptr;
    void CreateHUDElements();
    void ShowInteractText(const std::string&, int = -1);
    void HideInteractMessage();
    void ShowAreaMessage(const std::string&);
    void UpdateElementPositions();
    void UpdateHP(int, int);
    void UpdateStatus(const std::string&);
    void Update();
};
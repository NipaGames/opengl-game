#pragma once

#include <core/ui/canvas.h>
#include <core/ui/component/textcomponent.h>
#include "components/animationcomponent.h"

class HUD {
private:
    float fadeAreaTextAway_;
    bool isAreaTextShown_ = false;
    float areaTextSeconds_ = 5.0f;
    std::vector<std::string> statuses_;
    float gameOverTextY_;
    float baseVignetteSize_;

    bool gameOver_ = false;
    float gameOverEffect_ = 0.0f;
    bool gameOverEffectFinished_ = false;
    float gameOverTime_ = 1.0f;
    bool gameOverContinue_ = false;
    float gameOverContinueEffect_ = 0.0f;
    bool gameOverContinueEffectFinished_ = false;
    float gameOverContinueTime_ = 1.0f;
    
public:
    std::string canvasId = "hud";
    UI::TextComponent* interactText = nullptr;
    UI::TextComponent* areaText = nullptr;
    UI::TextComponent* hpText = nullptr;
    UI::TextComponent* maxHpText = nullptr;
    UI::TextComponent* statusText = nullptr;
    FloatAnimation* areaAnimation = nullptr;
    UI::TextComponent* gameOverText = nullptr;
    UI::TextComponent* gameOverInstructionsText = nullptr;
    void CreateHUDElements();
    void ShowInteractText(const std::string&, int = -1);
    void HideInteractMessage();
    void ShowAreaMessage(const std::string&);
    void UpdateElementPositions();
    void GameOver();
    void UpdateHP(int, int);
    void UpdateStatusText();
    void AddStatus(const std::string&);
    void RemoveStatus(const std::string&);
    void Update();
};
#include "hud.h"
#include "game.h"
#include <core/input.h>

#include <sstream>

using namespace UI;

const std::unordered_map<std::string, std::string> INTERACT_MESSAGES = {
    { "INTERACT", "interact" },
    { "OPEN_GATE", "open the gate" },
    { "SPAWN", "return to the spawn" }
};

void HUD::CreateHUDElements() {
    Canvas& c = GAME->GetRenderer().CreateCanvas(canvasId);
    Entity& interactTextEntity = GAME->GetEntityManager().CreateEntity();
    interactTextEntity.transform->position.x = 10;
    interactTextEntity.transform->position.y = 15;
    interactTextEntity.transform->size.z = .75f;
    interactText = interactTextEntity.AddComponent<TextComponent>(&c);
    interactText->font = "FONT_MORRIS";
    interactText->isVisible = false;
    interactText->AddToCanvas();
    interactTextEntity.Start();

    Entity& hpTextEntity = GAME->GetEntityManager().CreateEntity();
    hpTextEntity.transform->position.x = 10;
    hpTextEntity.transform->position.y = 680;
    hpTextEntity.transform->size.z = .75f;
    hpText = hpTextEntity.AddComponent<TextComponent>(&c);
    hpText->font = "FONT_CELTIC";
    hpText->AddToCanvas();
    hpTextEntity.Start();
    
    Entity& maxHpTextEntity = GAME->GetEntityManager().CreateEntity();
    maxHpTextEntity.transform->position.y = hpTextEntity.transform->position.y;
    maxHpTextEntity.transform->size.z = .5f;
    maxHpText = maxHpTextEntity.AddComponent<TextComponent>(&c);
    maxHpText->color = glm::vec4(glm::vec3(1.0f), .75f);
    maxHpText->font = "FONT_CELTIC";
    maxHpText->AddToCanvas();
    maxHpTextEntity.Start();

    Entity& statusTextEntity = GAME->GetEntityManager().CreateEntity();
    statusTextEntity.transform->position.x = hpTextEntity.transform->position.x;
    statusTextEntity.transform->position.y = hpTextEntity.transform->position.y - 30;
    statusTextEntity.transform->size.z = .6f;
    statusText = statusTextEntity.AddComponent<TextComponent>(&c);
    statusText->font = "FONT_MORRIS";
    statusText->lineSpacing = 20;
    statusText->SetShader("STATUS_TEXT");
    statusText->AddToCanvas();
    statusTextEntity.Start();

    Entity& areaTextEntity = GAME->GetEntityManager().CreateEntity();
    areaTextEntity.transform->position.x = 1270;
    areaTextEntity.transform->position.y = 25;
    areaTextEntity.transform->size.z = 2.0f;
    areaText = areaTextEntity.AddComponent<TextComponent>(&c);
    areaText->font = "FONT_ENCHANTED";
    areaText->SetShader("AREA_TEXT");
    areaText->alignment = Text::TextAlignment::RIGHT;
    areaText->color = glm::vec4(glm::vec3(0.75f), 0.0f);
    areaText->AddToCanvas();

    areaAnimation = areaTextEntity.AddComponent<FloatAnimation>();
    areaAnimation->ptr = &areaText->color.w;
    areaAnimation->allowInterruptions = true;
    areaTextEntity.Start();

    Entity& gameOverTextEntity = GAME->GetEntityManager().CreateEntity();
    gameOverTextEntity.transform->position.x = 1280 / 2;
    gameOverTextEntity.transform->position.y = 720 / 2;
    gameOverTextEntity.transform->size.z = 3.0f;
    gameOverText = gameOverTextEntity.AddComponent<TextComponent>(&c);
    gameOverText->font = "FONT_ENCHANTED";
    gameOverText->alignment = Text::TextAlignment::CENTER;
    gameOverText->color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
    gameOverText->isVisible = false;
    gameOverText->SetText("game over");
    gameOverText->AddToCanvas();

    Entity& gameOverInstructionsTextEntity = GAME->GetEntityManager().CreateEntity();
    gameOverInstructionsTextEntity.transform->position.x = 1280 / 2;
    gameOverInstructionsTextEntity.transform->position.y = 720 / 2 - 100;
    gameOverInstructionsTextEntity.transform->size.z = 1.0f;
    gameOverInstructionsText = gameOverInstructionsTextEntity.AddComponent<TextComponent>(&c);
    gameOverInstructionsText->font = "FONT_MORRIS";
    gameOverInstructionsText->alignment = Text::TextAlignment::CENTER;
    gameOverInstructionsText->isVisible = false;
    gameOverInstructionsText->lineSpacing = 20;
    gameOverInstructionsText->SetText("[E] to retry");
    gameOverInstructionsText->AddToCanvas();

    GAME->GetGameWindow().eventHandler.Subscribe(WindowEvent::WINDOW_RESIZE, [this]() { 
        this->UpdateElementPositions();
    });
}

void HUD::UpdateElementPositions() {
    maxHpText->parent->transform->position.x = hpText->parent->transform->position.x + hpText->GetTextSize().x;
}

void HUD::UpdateHP(int hp, int maxHp) {
    hpText->SetText("HP: " + std::to_string(hp));
    maxHpText->SetText(" / " + std::to_string(maxHp));
    UpdateElementPositions();
}

void HUD::UpdateStatusText() {
    std::stringstream s;
    for (const std::string& status : statuses_) {
        s << status << "\n";
    }
    statusText->SetText(s.str());
}

void HUD::AddStatus(const std::string& status) {
    statuses_.push_back(status);
    UpdateStatusText();
}

void HUD::RemoveStatus(const std::string& status) {
    auto it = std::find(statuses_.begin(), statuses_.end(), status);
    if (it != statuses_.end()) {
        statuses_.erase(it);
        UpdateStatusText();
    }
}

void HUD::GameOver() {
    gameOverEffect_ = 0.0f;
    gameOverEffectFinished_ = false;
    gameOver_ = true;    
    gameOverContinue_ = false;
    gameOverTextY_ = gameOverText->parent->transform->position.y;
    gameOverText->parent->transform->position.y = 720.0f;
    gameOverText->color.a = 0.0f;
    gameOverText->isVisible = true;

    PostProcessing& postProcessing = MonkeyGame::GetGame()->postProcessing;
    postProcessing.ApplyKernel(Convolution::GaussianBlur<7>());
    postProcessing.kernel.vignette.isActive = false;
    baseVignetteSize_ = postProcessing.vignette.size;
    MonkeyGame::GetGame()->GetRenderer().ApplyPostProcessing(postProcessing);
}

void HUD::Update() {
    // yippee hardcoded animation interpolation
    // too lazy to write this more elegantly
    if (gameOver_ && !gameOverEffectFinished_) {
        gameOverEffect_ += (float) GAME->GetDeltaTime() / gameOverTime_;
        gameOverEffect_ = std::min(gameOverEffect_, 1.0f);
        
        PostProcessing& postProcessing = MonkeyGame::GetGame()->postProcessing;
        postProcessing.saturation = 1.0f - .75f * gameOverEffect_;
        postProcessing.kernel.offset = gameOverEffect_ / 1000.0f;
        postProcessing.vignetteColor = glm::vec3(gameOverEffect_ * .5f, 0.0f, 0.0f);
        postProcessing.vignette.size = baseVignetteSize_ - (baseVignetteSize_ - 0.65f) * gameOverEffect_;
        MonkeyGame::GetGame()->GetRenderer().ApplyPostProcessing(postProcessing);

        gameOverText->parent->transform->position.y = 720.0f - gameOverTextY_ * gameOverEffect_;
        gameOverText->color.a = gameOverEffect_;

        if (gameOverEffect_ >= 1.0f) {
            gameOverEffectFinished_ = true;
            GAME->GetGameWindow().LockMouse(false);
            gameOverInstructionsText->isVisible = true;
        }
    }
    if (gameOverContinue_ && !gameOverContinueEffectFinished_) {
        gameOverContinueEffect_ += (float) GAME->GetDeltaTime() / gameOverContinueTime_;
        gameOverContinueEffect_ = std::min(gameOverContinueEffect_, 1.0f);

        PostProcessing& postProcessing = MonkeyGame::GetGame()->postProcessing;
        postProcessing.brightness = 1.0f - gameOverContinueEffect_;
        postProcessing.kernel.offset = (1.0f + gameOverContinueEffect_) / 1000.0f;
        MonkeyGame::GetGame()->GetRenderer().ApplyPostProcessing(postProcessing);

        gameOverText->parent->transform->position.y = gameOverTextY_ * (1.0f - gameOverContinueEffect_);
        gameOverText->color.a = 1.0f - gameOverContinueEffect_;

        if (gameOverContinueEffect_ >= 1.0f) {
            gameOverContinueEffectFinished_ = true;
        }
    }
    if (gameOverEffectFinished_ && !gameOverContinue_) {
        if (Input::IsKeyPressedDown(GLFW_KEY_E)) {
            gameOverContinue_ = true;
            gameOverContinueEffect_ = 0.0f;
            gameOverInstructionsText->isVisible = false;

            areaText->isVisible = false;
            interactText->isVisible = false;
            hpText->isVisible = false;
            maxHpText->isVisible = false;
            statusText->isVisible = false;
        }
    }
    if (gameOverContinueEffectFinished_) {
        MonkeyGame::GetGame()->GameThreadCleanUp();
        MonkeyGame::GetGame()->GameThreadStart();
    }
    if (isAreaTextShown_ && glfwGetTime() >= fadeAreaTextAway_) {
        isAreaTextShown_ = false;
        areaAnimation->playReverse = true;
        areaAnimation->Play();
    }
}

void HUD::ShowInteractText(const std::string& msgId, int keyCode) {
    std::string msgPart;
    if (INTERACT_MESSAGES.count(msgId) == 0)
        msgPart = msgId;
    else
        msgPart = INTERACT_MESSAGES.at(msgId);
    std::stringstream msgStream;
    if (keyCode != -1) {
        const char* key;
        key = glfwGetKeyName(keyCode, glfwGetKeyScancode(keyCode));
        msgStream << "[";
        if (key == nullptr) {
            switch (keyCode) {
                case GLFW_KEY_LEFT_SHIFT:
                case GLFW_KEY_RIGHT_SHIFT:
                    msgStream << "SHIFT";
                    break;
                case GLFW_KEY_TAB:
                    msgStream << "TAB";
                    break;
            }
        }
        else {
            msgStream << (char) std::toupper(key[0]);
        }
        msgStream << "] ";
    }
    msgStream << msgPart;
    interactText->SetText(msgStream.str());
    interactText->isVisible = true;
}

void HUD::HideInteractMessage() {
    interactText->isVisible = false;
}

void HUD::ShowAreaMessage(const std::string& area) {
    areaText->SetText(area);
    areaAnimation->playReverse = false;
    areaAnimation->Play();
    isAreaTextShown_ = true;
    fadeAreaTextAway_ = (float) glfwGetTime() + areaTextSeconds_;
}
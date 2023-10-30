#include "hud.h"
#include "game.h"

using namespace UI;

const std::unordered_map<std::string, std::string> INTERACT_MESSAGES = {
    { "OPEN", "[E] to open" }
};

void HUD::CreateHUDElements() {
    Canvas& c = GAME->GetRenderer().CreateCanvas(canvasId);
    Entity& interactMsgEntity = GAME->GetEntityManager().CreateEntity();
    interactMsgEntity.transform->position.x = 10;
    interactMsgEntity.transform->position.y = 15;
    interactMsg = interactMsgEntity.AddComponent<TextComponent>(&c);
    interactMsg->font = fontId;
    interactMsg->isVisible = false;
    interactMsg->AddToCanvas();
    interactMsg->Start();
}

void HUD::ShowInteractMessage(const std::string& key) {
    if (INTERACT_MESSAGES.count(key) == 0)
        return;
    interactMsg->SetText(INTERACT_MESSAGES.at(key));
    interactMsg->isVisible = true;
}

void HUD::HideInteractMessage() {
    interactMsg->isVisible = false;
}
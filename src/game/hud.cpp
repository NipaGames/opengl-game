#include "hud.h"
#include "game.h"

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
    interactTextEntity.transform->size.z = .5f;
    interactText = interactTextEntity.AddComponent<TextComponent>(&c);
    interactText->font = fontId;
    interactText->isVisible = false;
    interactText->AddToCanvas();
    interactText->Start();

    Entity& areaTextEntity = GAME->GetEntityManager().CreateEntity();
    areaTextEntity.transform->position.x = 1270;
    areaTextEntity.transform->position.y = 25;
    areaTextEntity.transform->size.z = 2.5f;
    areaText = areaTextEntity.AddComponent<TextComponent>(&c);
    areaText->font = fontId;
    areaText->alignment = Text::TextAlignment::RIGHT;
    areaText->SetText("humongous text");
    areaText->AddToCanvas();
    areaText->Start();
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
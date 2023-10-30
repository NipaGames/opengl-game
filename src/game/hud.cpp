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
    Entity& interactMsgEntity = GAME->GetEntityManager().CreateEntity();
    interactMsgEntity.transform->position.x = 10;
    interactMsgEntity.transform->position.y = 20;
    interactMsgEntity.transform->size.z = .75f;
    interactMsg = interactMsgEntity.AddComponent<TextComponent>(&c);
    interactMsg->font = fontId;
    interactMsg->isVisible = false;
    interactMsg->AddToCanvas();
    interactMsg->Start();
}

void HUD::ShowInteractMessage(const std::string& msgId, int keyCode) {
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
    interactMsg->SetText(msgStream.str());
    interactMsg->isVisible = true;
}

void HUD::HideInteractMessage() {
    interactMsg->isVisible = false;
}
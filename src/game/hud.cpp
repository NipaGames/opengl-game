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
    interactTextEntity.transform->size.z = .75f;
    interactText = interactTextEntity.AddComponent<TextComponent>(&c);
    interactText->font = "FONT_MORRIS";
    interactText->isVisible = false;
    interactText->AddToCanvas();
    interactTextEntity.Start();

    Entity& areaTextEntity = GAME->GetEntityManager().CreateEntity();
    areaTextEntity.transform->position.x = 1270;
    areaTextEntity.transform->position.y = 25;
    areaTextEntity.transform->size.z = 2.0f;
    areaText = areaTextEntity.AddComponent<TextComponent>(&c);
    areaText->font = "FONT_ENCHANTED";
    areaText->SetShader(Shaders::ShaderID::AREA_TEXT);
    areaText->alignment = Text::TextAlignment::RIGHT;
    areaText->color = glm::vec4(glm::vec3(0.75f), 0.0f);
    areaText->AddToCanvas();

    areaAnimation = areaTextEntity.AddComponent<FloatAnimation>();
    areaAnimation->ptr = &areaText->color.w;
    areaAnimation->allowInterruptions = true;
    areaTextEntity.Start();
}

void HUD::Update() {
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
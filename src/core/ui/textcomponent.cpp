#include "core/ui/textcomponent.h"
#include "core/game.h"
#include <iostream>

void UI::TextComponent::Start() {
    shader_ = Shader(SHADER_UI_TEXT);
    game->GetRenderer().AddUIComponent(this);
}

void UI::TextComponent::Render(const glm::mat4& projection) {
    shader_.Use();
    shader_.SetUniform("textColor", glm::vec3(1.0));
    shader_.SetUniform("projection", projection);
    UI::Text::RenderText(font, "haha kakkapylly", glm::vec2(100, 100), 1.0f);
}
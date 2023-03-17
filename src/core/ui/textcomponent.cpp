#include "core/ui/textcomponent.h"

#include <algorithm>
#include "core/game.h"

void UI::TextComponent::Start() {
    shader_ = Shader(SHADER_UI_TEXT);
    game->GetRenderer().AddUIComponent(this);
}

void UI::TextComponent::Render(const glm::mat4& projection) {
    shader_.Use();
    shader_.SetUniform("textColor", color);
    shader_.SetUniform("projection", projection);
    glm::vec2 pos(parent->transform->position.x, parent->transform->position.y);
    // janky ass way to determine the size
    glm::vec3& sizeVec = parent->transform->size;
    float size = std::max({ sizeVec.x, sizeVec.y, sizeVec.z });

    UI::Text::RenderText(UI::Text::GetFont(font), text_, pos, size);
}

void UI::TextComponent::SetText(const std::string& t) {
    text_ = t;
}

const std::string& UI::TextComponent::GetText() {
    return text_;
}
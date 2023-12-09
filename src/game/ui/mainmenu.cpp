#include "mainmenu.h"
#include <core/ui/component/textcomponent.h>
#include <core/game.h>

UI::TextComponent* MainMenu::AddMainButton(const std::string& header) {
    UI::TextComponent* buttonText = AddUIComponent<UI::TextComponent>();
    buttonText->SetTransform({ glm::vec2(640, mainButtonY_), .8f });
    buttonText->font = "FONT_MORRIS";
    buttonText->color = glm::vec4(glm::vec3(.75f), 1.0f);
    buttonText->alignment = UI::Text::TextAlignment::CENTER;
    buttonText->SetText(header);
    mainButtonY_ -= 50;
    return buttonText;
}

void MainMenu::CreateHUDElements() {
    bgMaterial = std::make_shared<Material>();
    bgMaterial->SetShaderUniform("color", glm::vec4(glm::vec3(0.0f), 1.0f));
    bgVerticalAnchor = UI::CanvasBackgroundVerticalAnchor::OVER;
    GenerateBackgroundShape();

    logoShape_.numVertexAttributes = 4;
    logoShape_.stride = 4;
    logoShape_.GenerateVAO();
    float logoLeft = 280;
    float logoRight = 1000;
    float logoTop = 650;
    float logoBottom = 450;
    float logoVertices[] = {
        // pos                  // texCoords
        logoLeft, logoTop,      0.0f, 0.0f,
        logoLeft, logoBottom,   0.0f, 1.0f,
        logoRight, logoBottom,  1.0f, 1.0f,
        logoLeft, logoTop,      0.0f, 0.0f,
        logoRight, logoBottom,  1.0f, 1.0f,
        logoRight, logoTop,     1.0f, 0.0f
    };
    logoShape_.SetVertexData(logoVertices);
    logoMaterial_ = std::make_shared<Material>(Shaders::ShaderID::UI_SHAPE);
    logoMaterial_->SetShaderUniform("color", glm::vec4(1.0f));
    logoMaterial_->SetTexture(GAME->resources.textureManager.Get("LATREUS_LOGO"));

    AddMainButton("continue")->color.a = .65f;
    AddMainButton("load game");
    AddMainButton("new game");
    AddMainButton("settings");
    AddMainButton("quit");

    #ifdef VERSION_SPECIFIED
    UI::TextComponent* versionText = AddUIComponent<UI::TextComponent>();
    versionText->SetTransform({ glm::vec2(1277, 7), .35f });
    versionText->font = "FONT_FIRACODE";
    versionText->alignment = UI::Text::TextAlignment::RIGHT;
    versionText->color = glm::vec4(glm::vec3(.75f), 1.0f);
    bool debug = false;
    #ifdef DEBUG_BUILD
    debug = true;
    #endif
    versionText->SetText(fmt::format("v{}.{} {}", VERSION_MAJ, VERSION_MIN, debug ? "Debug" : "Release"));
    #endif
}

void MainMenu::Draw() {
    Canvas::Draw();
    logoMaterial_->GetShader().Use();
    logoMaterial_->GetShader().SetUniform("projection", GetProjectionMatrix());
    logoMaterial_->Use();
    logoMaterial_->BindTexture();
    logoShape_.Bind();
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void MainMenu::AssignToRenderer(Renderer& renderer) {
    renderer.AssignCanvas("MAIN_MENU", GetCanvas());
}
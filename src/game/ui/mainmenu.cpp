#include "mainmenu.h"
#include <game/game.h>

#ifdef _WIN32
#undef APIENTRY
#include <windows.h>
#include <shellapi.h>
#endif

UI::ButtonComponent* MainMenu::AddMainButton(const std::string& header) {
    UI::ButtonComponent* button = AddUIComponent<UI::ButtonComponent>();
    button->SetTransform({ glm::vec2(1280 / 2, mainButtonY_), .8f });
    button->font = "FONT_MORRIS";
    glm::vec3 baseColor = glm::vec3(.75f);
    glm::vec3 hoverColor = glm::vec3(1.0f);
    button->color = glm::vec4(baseColor, 1.0f);
    button->eventHandler.Subscribe("mouseEnter", [=]() {
        button->color = glm::vec4(hoverColor, button->color.a);
    });
    button->eventHandler.Subscribe("mouseLeave", [=]() {
        button->color = glm::vec4(baseColor, button->color.a);
    });
    button->alignment = UI::Text::TextAlignment::CENTER;
    button->SetText(header);
    mainButtonY_ -= 50;
    return button;
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

    UI::TextComponent* openglGameText = AddUIComponent<UI::TextComponent>();
    openglGameText->SetTransform({ glm::vec2(1280 / 2, 450), .35f });
    openglGameText->font = "FONT_MORRIS";
    openglGameText->alignment = UI::Text::TextAlignment::CENTER;
    openglGameText->color = glm::vec4(glm::vec3(.75f), .5f);
    openglGameText->SetText("formerly known as the great opengl-game");

    const auto startGame = [&]() {
        MonkeyGame::GetGame()->RequestEventNextUpdate([&]() {
            this->Destroy();
            MonkeyGame::GetGame()->StartGame();
        });
    };
    UI::ButtonComponent* continueBtn = AddMainButton("continue");
    continueBtn->color.a = .65f;
    continueBtn->isActive = false;
    UI::ButtonComponent* loadGameBtn = AddMainButton("load game");
    loadGameBtn->eventHandler.Subscribe("click", [=]() {
        MonkeyGame::GetGame()->SetNextStage("sanctuary-entrance");
        startGame();
    });
    UI::ButtonComponent* newGameBtn = AddMainButton("new game");
    newGameBtn->eventHandler.Subscribe("click", [=]() {
        MonkeyGame::GetGame()->SetNextStage("teststage");
        startGame();
    });
    UI::ButtonComponent* settingsBtn = AddMainButton("settings");
    settingsBtn->eventHandler.Subscribe("click", [&]() {
        #ifdef _WIN32
        std::string settingsPath = std::fs::absolute(Paths::USER_SETTINGS_DIR).string();
        ShellExecuteA(0, "open", (LPCSTR) settingsPath.c_str(), 0, 0, SW_SHOW);
        #endif
    });
    UI::ButtonComponent* quitBtn = AddMainButton("quit");
    quitBtn->eventHandler.Subscribe("click", [&]() {
        GAME->Quit();
    });

    #ifdef VERSION_SPECIFIED
    UI::TextComponent* versionText = AddUIComponent<UI::TextComponent>();
    versionText->SetTransform({ glm::vec2(1280 - 3, 7), .35f });
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

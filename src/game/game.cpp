#include "game/game.h"

#include "core/input.h"
#include "core/entity/component/meshrenderer.h"
#include "core/graphics/model.h"
#include "core/stage/stage.h"
#include "core/terrain/plane.h"
#include "core/ui/textcomponent.h"
#include "game/components/playercontroller.h"
#include "game/components/rotatecube.h"
#include "game/components/debugoverlay.h"

#define LOG_FN_(fn) spdlog::debug("[{} called]", fn)
#define LOG_FN() LOG_FN_(__FUNCTION__)

UI::TextComponent* fpsText = nullptr;
UI::Text::FontID fontId = -1;
UI::Text::FontID font2Id = -1;

bool MonkeyGame::InitWindow() {
    LOG_FN();
    window_ = GameWindow("apina peli !!!!!!", 1280, 720, false);
    if(!window_.Create(renderer_)) {
        return false;
    }
    return true;
}

void MonkeyGame::PreLoad() {
    LOG_FN();
    auto font = UI::Text::LoadFontFile("../res/fonts/Augusta.ttf", 96);
    auto font2 = UI::Text::LoadFontFile("../res/fonts/SEGOEUI.ttf", 48);
    if (font != std::nullopt)
        fontId = UI::Text::AssignFont(*font);
    if (font != std::nullopt)
        font2Id = UI::Text::AssignFont(*font2);
}

void MonkeyGame::Start() {
    LOG_FN();
    Stage::AddStage(Stage::ReadStageFromFile("../res/stages/test.json"));
    
    Entity& player = entityManager_.CreateEntity("Player");
    player.AddComponent<PlayerController>();

    float range = 4.0f;
    int monkeyCount = 6;
    Model monkeyModel;
    monkeyModel.LoadModel("../res/objects/chimp.fbx");
    for (auto mesh : monkeyModel.meshes) {
        mesh->GenerateVAO();
        mesh->material = std::make_shared<Material>(SHADER_LIT);
        glm::vec3 color = glm::vec3((double) rand() / (RAND_MAX), (double) rand() / (RAND_MAX), (double) rand() / (RAND_MAX));
        mesh->material->SetShaderUniform<glm::vec3>("color", color);
        mesh->material->SetShaderUniform<glm::vec3>("ambientColor", glm::vec3(.1f));
        mesh->material->SetShaderUniform<int>("specularHighlight", 32);
        mesh->material->SetShaderUniform<float>("specularStrength", 1.5f);
    }
    for (int i = 0; i < monkeyCount; i++) {
        Entity& monkey = entityManager_.CreateEntity();
        monkey.AddComponent<MeshRenderer>()->meshes = monkeyModel.meshes;
        monkey.AddComponent<RotateCube>();
        
        double rad = ((2 * M_PI) / monkeyCount) * i;
        monkey.transform->position = glm::vec3(cos(rad) * range, 0.0, sin(rad) * range);
        monkey.transform->size = glm::vec3(1.0f, 1.0f, .5f);
    }
    Model mogusModel;
    mogusModel.LoadModel("../res/objects/mog.obj");
    for (auto mesh : mogusModel.meshes) {
        mesh->GenerateVAO();
        mesh->material = std::make_shared<Material>(SHADER_UNLIT);
        mesh->material->SetShaderUniform<glm::vec3>("color", glm::vec3(0.0f, 0.0f, 0.0f));
    }
    mogusModel.meshes[2]->material->SetShaderUniform<glm::vec3>("color", glm::vec3(1.0f, 0.0f, 0.0f));
    mogusModel.meshes[3]->material->SetShaderUniform<glm::vec3>("color", glm::vec3(0.5f, 0.5f, 1.0f));

    Entity& mogus = entityManager_.CreateEntity();
    mogus.AddComponent<MeshRenderer>()->meshes = mogusModel.meshes;
    mogus.transform->position = glm::vec3(0.0f, 0.25f, 0.0f);
    mogus.transform->size = glm::vec3(.5f);

    auto plane = std::make_shared<Plane>(glm::ivec2(25, 25));
    plane->heightVariation = .25f;
    plane->textureSize = glm::vec2(2);
    plane->GenerateVertices();
    plane->GenerateVAO();
    plane->material = std::make_shared<Material>(SHADER_LIT, Texture::LoadTexture("../res/textures/ground.jpg"));
    plane->material->SetShaderUniform<int>("specularHighlight", 8);
    plane->material->SetShaderUniform<float>("specularStrength", 0);
    Entity& terrain = entityManager_.CreateEntity();
    terrain.AddComponent<MeshRenderer>()->meshes.push_back(plane);
    terrain.transform->size = glm::vec3(100, 2, 100);
    terrain.transform->position.y = -.5f;

    UI::Canvas& canvas = GetRenderer().CreateCanvas("test");
    canvas.isVisible = false;
    
    if (fontId != -1 && font2Id != -1) {
        auto debugOverlay = entityManager_.CreateEntity().AddComponent<DebugOverlay>();
        debugOverlay->fontId = font2Id;

        /*Entity& textEntity = entityManager_.CreateEntity();
        fpsText = textEntity.AddComponent<UI::TextComponent>(&canvas);
        fpsText->renderingMethod = UI::TextRenderingMethod::RENDER_EVERY_FRAME;
        fpsText->color = glm::vec4(1.0f);
        fpsText->font = fontId;
        fpsText->AddToCanvas();
        textEntity.transform->position.x = 25;
        textEntity.transform->position.y = 680;
        textEntity.transform->size.z = .5f;

        Entity& versionTextEntity = entityManager_.CreateEntity();
        UI::TextComponent* versionText = versionTextEntity.AddComponent<UI::TextComponent>(&canvas);
        versionText->font = font2Id;
        #ifdef VERSION_SPECIFIED
        versionText->SetText("v" + std::to_string(VERSION_MAJ) + "." + std::to_string(VERSION_MIN));
        #else
        versionText->SetText("[invalid version]");
        #endif
        versionText->color = glm::vec4(glm::vec3(1.0f), .75f);
        versionText->AddToCanvas();
        versionTextEntity.transform->position.x = 10;
        versionTextEntity.transform->position.y = 10;
        versionTextEntity.transform->size.z = .4f;*/

        // super text rendering benchmark 9000
        /*for (int i = 0; i < 100; i++) {
            Entity& testText = entityManager_.CreateEntity();
            auto textComponent = testText.AddComponent<UI::TextComponent>(&canvas);
            textComponent->AddToCanvas();
            textComponent->font = fontId;
            testText.transform->position.x = (i / 10) % 1280 * 128;
            testText.transform->position.y = (i % 10) * 72;
            testText.transform->size.x = 1.0f;
            textComponent->SetText(std::to_string(i));
        }*/
    }
    Stage::LoadStage("teststage");
}

void MonkeyGame::Update() {
    if (Input::IsKeyPressedDown(GLFW_KEY_N))
        GetRenderer().highlightNormals = !GetRenderer().highlightNormals;

    if (Input::IsKeyPressedDown(GLFW_KEY_L)) {
        Stage::UnloadAllStages();
        Stage::LoadStage("teststage");
    }

    if (Input::IsKeyPressedDown(GLFW_KEY_U)) {
        Stage::UnloadAllStages();
    }
}
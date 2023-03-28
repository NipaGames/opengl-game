#include "game/game.h"

#include "core/entity/component/meshrenderer.h"
#include "core/graphics/model.h"
#include "core/ui/textcomponent.h"
#include "core/input.h"
#include "core/terrain/plane.h"
#include "game/components/playercontroller.h"
#include "game/components/rotatecube.h"

UI::TextComponent* fpsText = nullptr;

bool MonkeyGame::Init() {
    window_ = GameWindow("apina peli !!!!!!", 1280, 720, false);
    if(!window_.Create(renderer_)) {
        return false;
    }
    return true;
}

void MonkeyGame::Start() {
    lastTime_ = glfwGetTime();
    frames_ = 0;

    Entity& player = entityManager_.CreateEntity();
    player.AddComponent<PlayerController>();

    renderer_.directionalLights.push_back({ glm::normalize(glm::vec3(1.0, 1.0, 0.0)), glm::vec3(1.0, 1.0, 1.0), 1.0 });

    float range = 4.0f;
    int monkeyCount = 6;
    Model monkeyModel;
    monkeyModel.LoadModel("../res/chimp.fbx");
    for (auto mesh : monkeyModel.meshes) {
        mesh->GenerateVAO();
        mesh->material = std::make_shared<Material>(SHADER_LIT);
        glm::vec3 color = glm::vec3((double) rand() / (RAND_MAX), (double) rand() / (RAND_MAX), (double) rand() / (RAND_MAX));
        mesh->material->SetShaderUniform<glm::vec3>("color", color);
        mesh->material->SetShaderUniform<glm::vec3>("ambientColor", glm::vec3(0.25f));
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
    mogusModel.LoadModel("../res/mog.obj");
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
    plane->material = std::make_shared<Material>(SHADER_LIT, Texture::LoadTexture("../res/ground.jpg"));
    plane->material->SetShaderUniform<int>("specularHighlight", 8);
    plane->material->SetShaderUniform<float>("specularStrength", 0);
    Entity& terrain = entityManager_.CreateEntity();
    terrain.AddComponent<MeshRenderer>()->meshes.push_back(plane);
    terrain.transform->size = glm::vec3(100, 2, 100);
    terrain.transform->position.y = -.5f;


    auto font = UI::Text::LoadFontFile("../res/Augusta.ttf", 48);
    if (font != std::nullopt) {
        auto fontId = UI::Text::AssignFont(*font);
        Entity& textEntity = entityManager_.CreateEntity();
        fpsText = textEntity.AddComponent<UI::TextComponent>();
        fpsText->font = fontId;
        textEntity.transform->position.x = 25;
        textEntity.transform->position.y = 25;
        textEntity.transform->size.x = 1.0f;

        // free way to crash your computer below

        // probably a good way to render this shit would be to pre-render
        // the textures every time the text is changed (not very often)
        // also shows how i haven't really implemented any order for depth buffer blending

        /*for (int i = 0; i < 1000; i++) {
            Entity& testText = entityManager_.CreateEntity();
            auto textComponent = testText.AddComponent<UI::TextComponent>();
            textComponent->font = fontId;
            testText.transform->position.x = rand() % 1280;
            testText.transform->position.y = rand() % 720;
            testText.transform->size.x = 1.0f;
            textComponent->SetText(std::to_string(i));
        }*/
    }
}

void MonkeyGame::Update() {
    if (Input::IsKeyPressedDown(GLFW_KEY_N))
        game->GetRenderer().highlightNormals = !game->GetRenderer().highlightNormals;
    
    frames_++;
    if (glfwGetTime() - lastTime_ >= 1.0) {
        spdlog::info("{} fps", double(frames_));
        if (fpsText != nullptr)
            fpsText->SetText(std::to_string(frames_) + " fps");
        frames_ = 0;
        lastTime_ += 1.0;
    }
}
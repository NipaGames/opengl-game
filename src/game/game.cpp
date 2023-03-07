#include "game/game.h"

#include "core/entity/component/meshrenderer.h"
#include "core/graphics/model.h"
#include "core/terrain/plane.h"
#include "game/components/playercontroller.h"
#include "game/components/rotatecube.h"

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
    renderer_.directionalLights.push_back({ glm::normalize(glm::vec3(-1.0, -1.0, 0.0)), glm::vec3(1.0, 1.0, 1.0), .25 });

    float range = 7.5f;
    int monkeyCount = 6;
    Model monkeyModel;
    monkeyModel.LoadModel("../res/chimp.fbx");
    for (auto mesh : monkeyModel.meshes) {
        mesh->GenerateVAO();
        mesh->material = std::make_shared<Material>(SHADER_LIT, Texture::LoadTexture("../res/Grass_Texture.png"));
        glm::vec3 color = glm::vec3((double) rand() / (RAND_MAX), (double) rand() / (RAND_MAX), (double) rand() / (RAND_MAX));
        mesh->material->SetShaderUniform<glm::vec3>("color", color);
        mesh->material->SetShaderUniform<glm::vec3>("ambientColor", glm::vec3(0.2));
        mesh->material->SetShaderUniform<int>("specularHighlight", 32);
        mesh->material->SetShaderUniform<float>("specularStrength", 1.0);
    }
    for (int i = 0; i < monkeyCount; i++) {
        Entity& monkey = entityManager_.CreateEntity();
        monkey.AddComponent<MeshRenderer>()->meshes = monkeyModel.meshes;
        monkey.AddComponent<RotateCube>();
        
        double rad = ((2 * M_PI) / monkeyCount) * i;
        monkey.transform->position = glm::vec3(cos(rad) * range, 1.0, sin(rad) * range);
        monkey.transform->size = glm::vec3(2.0f);
    }
    Model mogusModel;
    mogusModel.LoadModel("../res/mog.obj");
    for (auto mesh : mogusModel.meshes) {
        mesh->GenerateVAO();
        mesh->material = std::make_shared<Material>(SHADER_UNLIT, Texture::LoadTexture("../res/Grass_Texture.png"));
        mesh->material->SetShaderUniform<glm::vec3>("color", glm::vec3(0.0f, 0.0f, 0.0f));
    }
    mogusModel.meshes[2]->material->SetShaderUniform<glm::vec3>("color", glm::vec3(1.0f, 0.0f, 0.0f));
    mogusModel.meshes[3]->material->SetShaderUniform<glm::vec3>("color", glm::vec3(0.5f, 0.5f, 1.0f));

    Entity& mogus = entityManager_.CreateEntity();
    mogus.AddComponent<MeshRenderer>()->meshes = mogusModel.meshes;
    mogus.transform->position = glm::vec3(0.0f, 2.0f, 0.0f);

    auto plane = std::make_shared<Plane>(glm::ivec2(25, 25));
    plane->GenerateVertices();
    plane->GenerateVAO();
    plane->material = std::make_shared<Material>(SHADER_LIT, Texture::LoadTexture("../res/Grass_Texture.png"));
    glm::vec3 color = glm::vec3(0.25, 1.0, 0.5);
    plane->material->SetShaderUniform<glm::vec3>("color", color);
    plane->material->SetShaderUniform<glm::vec3>("ambientColor", glm::vec3(0.0));
    plane->material->SetShaderUniform<int>("specularHighlight", 8);
    plane->material->SetShaderUniform<float>("specularStrength", 0);
    Entity& terrain = entityManager_.CreateEntity();
    terrain.AddComponent<MeshRenderer>()->meshes.push_back(plane);
    terrain.transform->size = glm::vec3(100, 2, 100);
    terrain.transform->position.y = -2.0f;
}

void MonkeyGame::Update() {
    frames_++;
    if (glfwGetTime() - lastTime_ >= 1.0) {
        spdlog::info("{} fps", double(frames_));
        frames_ = 0;
        lastTime_ += 1.0;
    }
}
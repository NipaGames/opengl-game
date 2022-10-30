#include "core/game.h"

#include <opengl.h>

#include "core/entity/component/meshrenderer.h"
#include "core/entity/component/playercontroller.h"
#include "core/entity/component/rotatecube.h"
#include "core/graphics/model.h"
#include "core/input.h"

bool Game::Init() {
    window_ = GameWindow("peli", 1280, 720);
    if(!window_.Create(renderer_)) {
        return false;
    }
    return true;
}

void Game::Run() {
    spdlog::info("Started running the game!");
    running_ = true;
    frames_ = 0;
    lastTime_ = glfwGetTime();
    deltaTime_ = lastTime_;

    // Run rendering, updates etc. in a separate thread
    // This way polling doesn't interrupt the whole program
    std::thread gameThread(&Game::GameThread, this);
    window_.SetupInputSystem();

    while(running_) {
        window_.UpdateInputSystem();
    }
    gameThread.join();
}

void Game::Start() {
    Entity player;
    player.AddComponent<PlayerController>();
    entityManager_.entities.push_back(player);

    Entity light;
    light.transform->position = glm::vec3(0.0, 10.0, 0.0);
    renderer_.directionalLights.push_back({ glm::normalize(glm::vec3(1.0, 1.0, 0.0)), glm::vec3(1.0, 1.0, 1.0), 1.0 });
    renderer_.directionalLights.push_back({ glm::normalize(glm::vec3(-1.0, -1.0, 0.0)), glm::vec3(1.0, 1.0, 1.0), .25 });
    entityManager_.entities.push_back(light);

    float range = 7.5f;
    int monkeyCount = 6;
    Model monkeyModel;
    monkeyModel.LoadModel("../res/chimp.fbx");
    for (auto mesh : monkeyModel.meshes) {
        mesh->GenerateVAO();
        mesh->material = std::make_shared<Material>(SHADER_LIT);
        glm::vec3 color = glm::vec3((double) rand() / (RAND_MAX), (double) rand() / (RAND_MAX), (double) rand() / (RAND_MAX));
        mesh->material->SetShaderUniform<glm::vec3>("color", color);
        mesh->material->SetShaderUniform<glm::vec3>("ambientColor", glm::vec3(0.0));
        mesh->material->SetShaderUniform<int>("specularHighlight", 32);
        mesh->material->SetShaderUniform<float>("specularStrength", 1.0);
    }
    for (int i = 0; i < monkeyCount; i++) {
        Entity monkey;
        auto meshRenderer = monkey.AddComponent<MeshRenderer>();
        meshRenderer->meshes = monkeyModel.meshes;
        monkey.AddComponent<RotateCube>();
        
        float rad = ((2 * M_PI) / monkeyCount) * i;
        monkey.transform->position = glm::vec3(cos(rad) * range, 1.0, sin(rad) * range);
        monkey.transform->size = glm::vec3(2.0f);
        entityManager_.entities.push_back(monkey);
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

    Entity mogus;
    auto meshRenderer = mogus.AddComponent<MeshRenderer>();
    meshRenderer->meshes = mogusModel.meshes;
    mogus.transform->position = glm::vec3(0.0f, 2.0f, 0.0f);
    entityManager_.entities.push_back(mogus);
}

void Game::Update() {
    if (glfwWindowShouldClose(window_.GetWindow())) {
        running_ = false;
        return;
    }

    double currentTime = glfwGetTime();
    deltaTime_ = currentTime - lastFrame_;
    lastFrame_ = currentTime;
    frames_++;
    if (currentTime - lastTime_ >= 1.0) {
        spdlog::info("{} fps", double(frames_));
        frames_ = 0;
        lastTime_ += 1.0;
    }

    if(Input::WINDOW_SIZE_CHANGE_PENDING) {
        int width, height;
        glfwGetFramebufferSize(window_.GetWindow(), &width, &height);
        renderer_.UpdateCameraProjection(width, height);
        Input::WINDOW_SIZE_CHANGE_PENDING = false;
    }

    window_.Update();

    for (Entity& entity : entityManager_.entities) {
        entity.Update();
    }

    Camera& cam = renderer_.GetCamera();
    glm::vec3 direction;
    direction.x = cos(glm::radians(cam.yaw)) * cos(glm::radians(cam.pitch));
    direction.y = sin(glm::radians(cam.pitch));
    direction.z = sin(glm::radians(cam.yaw)) * cos(glm::radians(cam.pitch));
    cam.front = glm::normalize(direction);
    renderer_.Render();
    Input::ClearKeysPressedDown();
}

void Game::GameThread() {
    glfwMakeContextCurrent(window_.GetWindow());
    Start();
    for (Entity& entity : entityManager_.entities) {
        entity.Start();
    }
    while (running_) {
        Update();
    }
}
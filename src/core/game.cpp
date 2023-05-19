#include "game.h"

#include <opengl.h>

#include "input.h"
#include <core/physics/physics.h>

bool Game::InitWindow() {
    window_ = GameWindow("", BASE_WIDTH, BASE_HEIGHT);
    if(!window_.Create(renderer_)) {
        return false;
    }
    return true;
}

void Game::Run() {
    running_ = true;
    deltaTime_ = glfwGetTime();

    // Run rendering, updates etc. in a separate thread
    // This way polling doesn't interrupt the whole program
    std::thread gameThread(&Game::GameThread, this);
    window_.SetupInputSystem();

    while(running_) {
        window_.UpdateInputSystem();
    }
    gameThread.join();
}

void Game::GameThread() {
    srand(static_cast<unsigned int>(time(0)));
    glfwMakeContextCurrent(window_.GetWindow());
    PreLoad();
    glfwShowWindow(game->GetGameWindow().GetWindow());
    Physics::Init();
    Start();
    for (const auto& entity : entityManager_.entities_) {
        entity->Start();
    }
    renderer_.Start();
    prevUpdate_ = glfwGetTime();
    prevFixedUpdate_ = prevUpdate_;
    while (running_) {
        if (glfwWindowShouldClose(window_.GetWindow())) {
            running_ = false;
            break;
        }
        
        double currentTime = glfwGetTime();
        if (limitFps_ > 0 && currentTime - prevUpdate_ < 1.0 / limitFps_)
            continue;
        deltaTime_ = currentTime - prevUpdate_;
        prevUpdate_ = currentTime;

        if (Input::SET_FULLSCREEN_PENDING) {
            const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
            game->GetRenderer().UpdateCameraProjection(mode->width, mode->height);
            Input::SET_FULLSCREEN_PENDING = false;
            Input::WINDOW_SIZE_CHANGE_PENDING = false;
        }
        if (Input::WINDOW_SIZE_CHANGE_PENDING) {
            int width, height;
            glfwGetFramebufferSize(window_.GetWindow(), &width, &height);
            if (width > 0 && height > 0)
                renderer_.UpdateCameraProjection(width, height);
            Input::WINDOW_SIZE_CHANGE_PENDING = false;
        }

        if (currentTime - prevFixedUpdate_ > 1.0 / fixedUpdateRate_) {
            Input::PollKeysPressedDown();
            prevFixedUpdate_ = currentTime;
            Physics::Update(GetFixedDeltaTime());
            for (const auto& entity : entityManager_.entities_) {
                entity->FixedUpdate();
            }
        }
        window_.Update();
        Update();
        for (const auto& entity : entityManager_.entities_) {
            entity->Update();
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
    Physics::Destroy();
}
#include "core/game.h"

#include <opengl.h>

#include "core/input.h"

bool Game::Init() {
    window_ = GameWindow("peli", 1280, 720);
    if(!window_.Create(renderer_)) {
        return false;
    }
    return true;
}

void Game::Run() {
    running_ = true;
    frames_ = 0;
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
    glfwMakeContextCurrent(window_.GetWindow());
    Start();
    for (Entity& entity : entityManager_.entities) {
        entity.Start();
    }
    while (running_) {
        if (glfwWindowShouldClose(window_.GetWindow())) {
            running_ = false;
            return;
        }
        
        double currentTime = glfwGetTime();
        deltaTime_ = currentTime - lastFrame_;
        lastFrame_ = currentTime;

        if(Input::WINDOW_SIZE_CHANGE_PENDING) {
            int width, height;
            glfwGetFramebufferSize(window_.GetWindow(), &width, &height);
            renderer_.UpdateCameraProjection(width, height);
            Input::WINDOW_SIZE_CHANGE_PENDING = false;
        }

        window_.Update();
        Update();
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
}
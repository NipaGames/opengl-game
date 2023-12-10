#include "game.h"

#include <opengl.h>

#include "input.h"
#include "physics/physics.h"
#include "io/serializetypes.h"

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

void Game::GameThreadInit() {
    srand(static_cast<unsigned int>(time(0)));
    glfwMakeContextCurrent(window_.GetWindow());
    PreLoad();
    glfwShowWindow(GAME->GetGameWindow().GetWindow());
    RegisterDefaultSerializers();
    resources.LoadAll();
    renderer_.UpdateVideoSettings(resources.videoSettings);
    window_.UseVsync(resources.videoSettings.useVsync);
    glfwSetWindowSize(window_.GetWindow(), resources.videoSettings.resolution.x, resources.videoSettings.resolution.y);
    if (resources.videoSettings.fullscreen)
        Input::UPDATE_FULLSCREEN = true;
}

void Game::StartEntities() {
    for (const auto& entity : entityManager_.entities_) {
        entity->Start();
    }
    renderer_.Start();
}

void Game::GameThreadStart() {
    Physics::Init();
    Start();
    StartEntities();
    prevUpdate_ = glfwGetTime();
    prevFixedUpdate_ = prevUpdate_;
}

void Game::GameThreadCleanUp() {
    CleanUp();
    Physics::Destroy();
    window_.eventHandler.ClearEvents();
    renderer_.CleanUpEntities();
    resources.stageManager.UnloadAllStages();
    entityManager_.ClearEntities();
}

void Game::GameThreadUpdate() {
    if (glfwWindowShouldClose(window_.GetWindow())) {
        running_ = false;
        return;
    }
    
    double currentTime = glfwGetTime();
    if (limitFps_ > 0 && currentTime - prevUpdate_ < 1.0 / limitFps_)
        return;
    // don't skip too big intervals (>.5s)
    deltaTime_ = std::min(currentTime - prevUpdate_, .5);
    if (freezeDeltaTime_) {
        deltaTime_ = 0;
        freezeDeltaTime_ = false;
    }
    prevUpdate_ = currentTime;

    if (Input::SET_FULLSCREEN_PENDING) {
        const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
        GAME->GetRenderer().UpdateCameraProjection(mode->width, mode->height);
        window_.eventHandler.Dispatch(WindowEvent::WINDOW_RESIZE);
        Input::SET_FULLSCREEN_PENDING = false;
        Input::WINDOW_SIZE_CHANGE_PENDING = false;
    }
    if (Input::WINDOW_SIZE_CHANGE_PENDING) {
        int width, height;
        glfwGetFramebufferSize(window_.GetWindow(), &width, &height);
        if (width > 0 && height > 0)
            renderer_.UpdateCameraProjection(width, height);
        window_.eventHandler.Dispatch(WindowEvent::WINDOW_RESIZE);
        Input::WINDOW_SIZE_CHANGE_PENDING = false;
    }
    
    if (Physics::dynamicsWorld != nullptr)
        Physics::Update(deltaTime_);
    
    isFixedUpdate_ = currentTime - prevFixedUpdate_ > 1.0 / fixedUpdateRate_;
    if (isFixedUpdate_) {
        Input::PollKeysPressedDown();
        Input::PollMouseButtonsPressedDown();
        prevFixedUpdate_ = currentTime;
        FixedUpdate();
        for (const auto& entity : entityManager_.entities_) {
            entity->FixedUpdate();
        }
        renderer_.UpdateFrustum();
        renderer_.SortMeshesByDistance();
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
    cam.right = glm::normalize(glm::cross(cam.front, cam.up));
    cam.UpdateFrustum();
    
    renderer_.Render();
    Input::ClearKeysPressedDown();
    Input::ClearMouseButtonsPressedDown();
}

void Game::GameThread() {
    GameThreadInit();
    GameThreadStart();
    while (running_) {
        GameThreadUpdate();
    }
    GameThreadCleanUp();
}

void Game::Quit() {
    running_ = false;
}
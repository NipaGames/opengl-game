#include "gamewindow.h"

#include "entity/component/meshrenderer.h"
#include "entity/component/playercontroller.h"
#include "entity/component/rotatecube.h"
#include "graphics/model.h"
#include "graphics/shader.h"
#include "graphics/shaders.h"
#include "input.h"

#include <atomic>
#include <glm/glm.hpp>
#include <thread>
#include <unordered_map>

namespace Input {
void KeyDown(int key){
    std::lock_guard<std::mutex> lock(keysMutex_);
    keys_[key] = true;
    keysPressedBeforePoll_[key] = true;
}

void KeyUp(int key){
    std::lock_guard<std::mutex> lock(keysMutex_);
    keys_[key] = false;
    keysPressed_[key] = false;
}

bool IsKeyDown(int key){
    std::lock_guard<std::mutex> lock(keysMutex_);
    if (!keys_.count(key))
        return false;
    return keys_[key];
}

bool IsKeyPressedDown(int key){
    std::lock_guard<std::mutex> lock(keysMutex_);
    if (!keysPressed_.count(key))
        return false;
    bool val = keysPressed_[key];
    return val;
}


void ClearKeysPressedDown() {
    std::lock_guard<std::mutex> lock(keysMutex_);
    keysPressed_.clear();
}

void PollKeysPressedDown() {
    std::lock_guard<std::mutex> lock(keysMutex_);
    keysPressed_.clear();
    keysPressed_ = keysPressedBeforePoll_;
    keysPressedBeforePoll_.clear();
}
}

GameWindow::GameWindow(const std::string& title, int w, int h) {
    title_ = title;
    baseWidth_ = w;
    baseHeight_ = h;
}

bool GameWindow::Create() {
    if(!glfwInit()) {
        spdlog::critical("GLFW init failed.");
        return false;
    } else {
        spdlog::info("GLFW init successful.");
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window_ = glfwCreateWindow(baseWidth_, baseHeight_, title_.c_str(), NULL, NULL);
    if(!window_) {
        spdlog::critical("GLFW window init failed. Make sure your GPU is OpenGL 3.3 compatible.");
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(window_);
    glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetInputMode(window_, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

    glewExperimental = true;
    if(glewInit() != GLEW_OK) {
        spdlog::critical("GLEW init failed. {}", glGetError());
        return false;
    } else {
        spdlog::info("GLEW init successful.");
    }

    spdlog::info("OpenGL version: {}", glGetString(GL_VERSION));

    renderer.SetWindow(window_);
    if(!renderer.Init())
        return false;

    Shaders::LoadShaders(SHADER_EXAMPLE, "example.vert", "example.frag");

    auto player = std::make_shared<Entity>(); 
    player->AddComponent<PlayerController>();
    entities.push_back(player);

    float range = 10.0f;
    meshes.insert(std::make_pair(Meshes::CUBE.id, Meshes::CreateMeshInstance(Meshes::CUBE)));
    for (int i = 0; i < 50; i++) {
        auto cube = std::make_shared<Entity>();
        auto meshRenderer = cube->AddComponent<MeshRenderer>();
        Mesh mesh = *meshes.at(Meshes::CUBE.id);
        mesh.material = Material(SHADER_EXAMPLE);
        meshRenderer->meshes.push_back(Meshes::CreateMeshInstance(Meshes::CUBE));
        cube->AddComponent<RotateCube>();
        
        cube->transform->position = glm::vec3((rand() / (RAND_MAX / range)) - range / 2, (rand() / (RAND_MAX / range)) - range / 2, (rand() / (RAND_MAX / range)) - range / 2);
        cube->transform->rotation = glm::quat(glm::vec3(rand() / (RAND_MAX / 360.0f), rand() / (RAND_MAX / 360.0f), rand() / (RAND_MAX / 360.0f)));
        entities.push_back(cube);
    }
    Model model;
    model.LoadModel("teapot.obj");

    glfwSwapInterval(1);
    glfwMakeContextCurrent(nullptr);
    return true;
}

void GameWindow::Update() {
    if (glfwWindowShouldClose(window_)) {
        running_ = false;
        return;
    }
    Input::PollKeysPressedDown();
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
        glfwGetFramebufferSize(window_, &width, &height);
        game.renderer.UpdateCameraProjection(width, height);
        Input::WINDOW_SIZE_CHANGE_PENDING = false;
    }

    if(Input::MOUSE_MOVE_PENDING) {
        Input::MOUSE_MOVE_PENDING = false;
        DispatchEvent(EventType::MOUSE_MOVE);
    }

    const float cameraSpeed = 15 * game.GetDeltaTime();
    auto& cam = renderer.GetCamera();
    glm::vec3 front = cam.front;
    front.y = 0.0f;
    front = glm::normalize(front);

    if (Input::IsKeyPressedDown(GLFW_KEY_ESCAPE)) {
        Input::IS_MOUSE_LOCKED = !Input::IS_MOUSE_LOCKED;
        Input::CURSOR_MODE_CHANGE_PENDING = true;
    }

    if (Input::IS_MOUSE_LOCKED) {
        glm::vec3 horizontalVelocity(0.0f);
        if (Input::IsKeyDown(GLFW_KEY_W))
            horizontalVelocity += front;
        if (Input::IsKeyDown(GLFW_KEY_S))
            horizontalVelocity -= front;
        if (Input::IsKeyDown(GLFW_KEY_A))
            horizontalVelocity -= glm::normalize(glm::cross(front, cam.up));
        if (Input::IsKeyDown(GLFW_KEY_D))
            horizontalVelocity += glm::normalize(glm::cross(front, cam.up));
            
        if (horizontalVelocity != glm::vec3(0.0f))
            horizontalVelocity = glm::normalize(horizontalVelocity);

        cam.pos += horizontalVelocity * cameraSpeed;
        if (Input::IsKeyDown(GLFW_KEY_SPACE))
            cam.pos += cameraSpeed * cam.up;
        if (Input::IsKeyDown(GLFW_KEY_LEFT_SHIFT))
            cam.pos -= cameraSpeed * cam.up;
    } else {
        if (glfwGetMouseButton(window_, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
            Input::IS_MOUSE_LOCKED = true;
            Input::CURSOR_MODE_CHANGE_PENDING = true;
        }
    }

    for (auto entity : entities) {
        entity->Update();
    }

    renderer.Render();
    Input::ClearKeysPressedDown();
}

void GameWindow::ResetCursorPos() {
    int width, height;
    glfwGetWindowSize(window_, &width, &height);
    glfwSetCursorPos(window_, width / 2, height / 2);
}

void GameWindow::DispatchEvent(EventType eventType) {
    for (auto event : events_) {
        if (event.first == eventType)
            event.second();
    }
}

void GameWindow::OnEvent(EventType eventType, std::function<void()> event) {
    events_.insert({ eventType, event });
}

void FramebufferSizeCallback(GLFWwindow* window, int width, int height) {
    Input::WINDOW_SIZE_CHANGE_PENDING = true;
}

void MouseCallback(GLFWwindow* window, double xPos, double yPos) {
    Input::MOUSE_MOVE_PENDING = true;
}

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
        Input::KeyDown(key);
        if (key == GLFW_KEY_F11 || (key == GLFW_KEY_ENTER && glfwGetKey(window, GLFW_KEY_LEFT_ALT)))
            Input::UPDATE_FULLSCREEN = true;
    }
    else if (action == GLFW_RELEASE) {
        Input::KeyUp(key);
    }
}

void GameWindow::GameThread() {
    glfwMakeContextCurrent(window_);
    for (auto entity : entities) {
        entity->Start();
    }
    while (running_) {
        Update();
    }
}

void GameWindow::Run() {
    spdlog::info("Started running the game!");

    running_ = true;
    frames_ = 0;
    lastTime_ = glfwGetTime();
    deltaTime_ = lastTime_;

    Input::IS_MOUSE_LOCKED = true;
    Input::MOUSE_MOVE_PENDING = true;

    // Run rendering, updates etc. in a separate thread
    // This way polling doesn't interrupt the whole program
    std::thread gameThread(&GameWindow::GameThread, this);
    glfwSetFramebufferSizeCallback(window_, FramebufferSizeCallback);
    glfwSetKeyCallback(window_, KeyCallback);
    glfwSetCursorPosCallback(window_, MouseCallback);
    while (running_) {
        glfwWaitEvents();
        if (Input::UPDATE_FULLSCREEN) {
            isFullscreen_ = !isFullscreen_;
            if ((glfwGetWindowMonitor(window_) != nullptr) != isFullscreen_) {
                if (isFullscreen_) {
                    glfwGetWindowPos(window_, &prevWndPos_.x, &prevWndPos_.y);
                    glfwGetWindowSize(window_, &prevWndSize_.x, &prevWndSize_.y);
                    const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
                    glfwSetWindowMonitor(window_, glfwGetPrimaryMonitor(), 0, 0, mode->width, mode->height, mode->refreshRate);
                    glfwSwapInterval(1);
                }
                else {
                    glfwSetWindowMonitor(window_, nullptr,  prevWndPos_.x, prevWndPos_.y, prevWndSize_.x, prevWndSize_.y, 0);
                }
                glm::tvec2<int> framebufferSize;
                glfwGetFramebufferSize(window_, &framebufferSize.x, &framebufferSize.y);
                glViewport(0, 0, framebufferSize.x, framebufferSize.y);
            }
            Input::UPDATE_FULLSCREEN = false;
        }
        if (Input::CURSOR_MODE_CHANGE_PENDING) {
            glfwSetInputMode(window_, GLFW_CURSOR, Input::IS_MOUSE_LOCKED ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
            if (Input::IS_MOUSE_LOCKED) {
                Input::FIRST_MOUSE = true;
            }
            Input::CURSOR_MODE_CHANGE_PENDING = false;
        }
    }
    gameThread.join();
}
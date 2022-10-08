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

    Shaders::LoadAllShaders();

    auto player = std::make_shared<Entity>(); 
    player->AddComponent<PlayerController>();
    entities.push_back(player);

    std::shared_ptr<Material> lightMaterial = std::make_shared<Material>(SHADER_UNLIT);
    lightMaterial->SetShaderUniform<glm::vec3>("objectColor", glm::vec3(1.0, 1.0, 1.0));

    auto light = std::make_shared<Entity>();
    auto mesh = Meshes::CreateMeshInstance(Meshes::CUBE);
    mesh->material = lightMaterial;
    light->AddComponent<MeshRenderer>()->meshes.push_back(mesh);
    light->transform->position = glm::vec3(0.0, 10.0, 0.0);
    entities.push_back(light);
    
    float range = 7.5f;
    int monkeyCount = 6;
    Model monkeyModel;
    monkeyModel.LoadModel("../res/chimp.fbx");
    for (auto mesh : monkeyModel.meshes) {
        mesh->GenerateVAO();
        mesh->material = std::make_shared<Material>(SHADER_LIT);
        glm::vec3 color = glm::vec3((double) rand() / (RAND_MAX), (double) rand() / (RAND_MAX), (double) rand() / (RAND_MAX));
        mesh->material->SetShaderUniform<glm::vec3>("objectColor", color);
        mesh->material->SetShaderUniform<glm::vec3>("lightPos", light->transform->position);
        mesh->material->SetShaderUniform<float>("lightRange", 10.0);
        mesh->material->SetShaderUniform<float>("specularStrength", 0.0);
    }
    for (int i = 0; i < monkeyCount; i++) {
        auto monkey = std::make_shared<Entity>();
        auto meshRenderer = monkey->AddComponent<MeshRenderer>();
        meshRenderer->meshes = monkeyModel.meshes;
        monkey->AddComponent<RotateCube>();
        
        float rad = ((2 * M_PI) / monkeyCount) * i;
        monkey->transform->position = glm::vec3(cos(rad) * range, 1.0, sin(rad) * range);
        
        monkey->transform->size = glm::vec3(2.0f);
        entities.push_back(monkey);
    }
    Model mogusModel;
    mogusModel.LoadModel("../res/mog.obj");
    for (auto mesh : mogusModel.meshes) {
        mesh->GenerateVAO();
        mesh->material = std::make_shared<Material>(SHADER_UNLIT);
        mesh->material->SetShaderUniform<glm::vec3>("objectColor", glm::vec3(0.0f, 0.0f, 0.0f));
    }
    mogusModel.meshes[2]->material->SetShaderUniform<glm::vec3>("objectColor", glm::vec3(1.0f, 0.0f, 0.0f));
    mogusModel.meshes[3]->material = std::make_shared<Material>(SHADER_LIT);

    mogusModel.meshes[3]->material->SetShaderUniform<glm::vec3>("objectColor", glm::vec3(0.5f, 0.5f, 1.0f));
    mogusModel.meshes[3]->material->SetShaderUniform<glm::vec3>("lightPos", light->transform->position);
    mogusModel.meshes[3]->material->SetShaderUniform<float>("lightRange", 10.0);
    mogusModel.meshes[3]->material->SetShaderUniform<int>("specularHighlight", 128);
    mogusModel.meshes[3]->material->SetShaderUniform<float>("specularStrength", 2.0);
    
    auto mogus = std::make_shared<Entity>();
    auto meshRenderer = mogus->AddComponent<MeshRenderer>();
    meshRenderer->meshes = mogusModel.meshes;
    mogus->transform->position = glm::vec3(0.0f, 2.0f, 0.0f);
    entities.push_back(mogus);

    glfwSwapInterval(1);
    glfwMakeContextCurrent(nullptr);
    return true;
}

void GameWindow::Update() {
    if (glfwWindowShouldClose(window_)) {
        running_ = false;
        return;
    }
    if (Input::VSYNC_POLL_RATE_PENDING) {
        glfwSwapInterval(1);
        Input::VSYNC_POLL_RATE_PENDING = false;
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

    if (Input::IsKeyPressedDown(GLFW_KEY_ESCAPE)) {
        Input::IS_MOUSE_LOCKED = !Input::IS_MOUSE_LOCKED;
        Input::CURSOR_MODE_CHANGE_PENDING = true;
    }

    if (glfwGetMouseButton(window_, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS && !Input::IS_MOUSE_LOCKED) {
        Input::IS_MOUSE_LOCKED = true;
        Input::CURSOR_MODE_CHANGE_PENDING = true;
    }

    for (auto entity : entities) {
        entity->Update();
    }

    Camera& cam = renderer.GetCamera();
    glm::vec3 direction;
    direction.x = cos(glm::radians(cam.yaw)) * cos(glm::radians(cam.pitch));
    direction.y = sin(glm::radians(cam.pitch));
    direction.z = sin(glm::radians(cam.yaw)) * cos(glm::radians(cam.pitch));
    cam.front = glm::normalize(direction);

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
                    Input::VSYNC_POLL_RATE_PENDING = true;
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
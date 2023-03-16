#include "core/gamewindow.h"

#include "core/graphics/shader.h"
#include "core/graphics/shaders.h"
#include "core/ui/text.h"
#include "core/input.h"
#include "core/game.h"

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

GameWindow::GameWindow(const std::string& title, int w, int h, bool useVsync) {
    title_ = title;
    baseWidth_ = w;
    baseHeight_ = h;
    useVsync_ = useVsync;
}

bool GameWindow::Create(Renderer& renderer) {
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

    renderer.skyboxColor = glm::vec3(.5, .75, 1.0);
    renderer.SetWindow(window_);
    if(!renderer.Init())
        return false;

    Shaders::LoadAllShaders();
    glfwSwapInterval(useVsync_ ? 1 : 0);
    glfwMakeContextCurrent(nullptr);

    if(!UI::Text::Init())
        return false;

    return true;
}

void GameWindow::Update() {
    if (Input::VSYNC_POLL_RATE_CHANGE_PENDING) {
        glfwSwapInterval(useVsync_ ? 1 : 0);
        Input::VSYNC_POLL_RATE_CHANGE_PENDING = false;
    }
    Input::PollKeysPressedDown();

    if(Input::MOUSE_MOVE_PENDING) {
        Input::MOUSE_MOVE_PENDING = false;
        if (Input::IS_MOUSE_LOCKED) {
            double xPos, yPos;
            glfwGetCursorPos(window_, &xPos, &yPos);

            if (Input::FIRST_MOUSE) {
                lastMouseX_ = xPos;
                lastMouseY_ = yPos;
                Input::FIRST_MOUSE = false;
            }

            Input::MOUSE_MOVE_X = xPos - lastMouseX_;
            Input::MOUSE_MOVE_Y = lastMouseY_ - yPos;

            lastMouseX_ = xPos;
            lastMouseY_ = yPos;

            DispatchEvent(EventType::MOUSE_MOVE);
        }
    }

    if (Input::IsKeyPressedDown(GLFW_KEY_ESCAPE)) {
        Input::IS_MOUSE_LOCKED = !Input::IS_MOUSE_LOCKED;
        Input::CURSOR_MODE_CHANGE_PENDING = true;
    }

    if (glfwGetMouseButton(window_, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS && !Input::IS_MOUSE_LOCKED) {
        Input::IS_MOUSE_LOCKED = true;
        Input::CURSOR_MODE_CHANGE_PENDING = true;
    }
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

void GameWindow::SetupInputSystem() {
    Input::IS_MOUSE_LOCKED = true;
    Input::MOUSE_MOVE_PENDING = true;

    glfwSetFramebufferSizeCallback(window_, FramebufferSizeCallback);
    glfwSetKeyCallback(window_, KeyCallback);
    glfwSetCursorPosCallback(window_, MouseCallback);
}

void GameWindow::UpdateInputSystem() {
    glfwWaitEvents();
    if (Input::UPDATE_FULLSCREEN) {
        isFullscreen_ = !isFullscreen_;
        if ((glfwGetWindowMonitor(window_) != nullptr) != isFullscreen_) {
            if (isFullscreen_) {
                glfwGetWindowPos(window_, &prevWndPos_.x, &prevWndPos_.y);
                glfwGetWindowSize(window_, &prevWndSize_.x, &prevWndSize_.y);
                const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
                if (useVsync_)
                    glfwSetWindowMonitor(window_, glfwGetPrimaryMonitor(), 0, 0, mode->width, mode->height, mode->refreshRate);
                else
                    glfwSetWindowMonitor(window_, glfwGetPrimaryMonitor(), 0, 0, mode->width, mode->height, GLFW_DONT_CARE);
                Input::VSYNC_POLL_RATE_CHANGE_PENDING = true;
            }
            else {
                glfwSetWindowMonitor(window_, nullptr,  prevWndPos_.x, prevWndPos_.y, prevWndSize_.x, prevWndSize_.y, 0);
            }
            glm::tvec2<int> windowSize;
            glfwGetWindowSize(window_, &windowSize.x, &windowSize.y);
            game->GetRenderer().UpdateCameraProjection(windowSize.x, windowSize.y);
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
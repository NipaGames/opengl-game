#pragma once

#include <opengl.h>
#include <spdlog/spdlog.h>
#include <iostream>
#include <cstdint>
#include <memory>
#include <unordered_map>
#include <vector>

#include "object.h"
#include "graphics/renderer.h"

class GameWindow {
private:
    GLFWwindow* window_ = nullptr;
    std::string title_;
    int baseWidth_, baseHeight_;
    bool running_ = false;
    bool isFullscreen_ = false;
    double lastTime_;
    int frames_;
    double deltaTime_;
    double lastFrame_;
    double lastMouseX_, lastMouseY_;
    glm::tvec2<int> prevWndPos_;
    glm::tvec2<int> prevWndSize_;
    bool updateViewport_ = false;
public:
    GameWindow() { }
    GameWindow(const std::string&, int, int);

    Renderer renderer;
    std::vector<std::shared_ptr<Object>> objects;
    std::unordered_map<std::string, const std::shared_ptr<Mesh>> meshes;
    bool Create();
    void Run();
    void Update();
    void GameThread();
    void ResetCursorPos();
    void OnMouseMove();
    const double& GetDeltaTime() { return deltaTime_; }
};

extern GameWindow game;
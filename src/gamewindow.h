#pragma once

#include <opengl.h>
#include <spdlog/spdlog.h>
#include <iostream>
#include <cstdint>
#include <memory>
#include <map>
#include <unordered_map>
#include <vector>

#include "entity/entity.h"
#include "event.h"
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
    glm::tvec2<int> prevWndPos_;
    glm::tvec2<int> prevWndSize_;
    std::multimap<EventType, std::function<void()>> events_;

    void DispatchEvent(EventType);
public:
    GameWindow() { }
    GameWindow(const std::string&, int, int);

    Renderer renderer;
    std::vector<std::shared_ptr<Entity>> entities;
    std::unordered_map<std::string, const std::shared_ptr<Mesh>> meshes;
    
    bool Create();
    void Run();
    void Update();
    void GameThread();
    void ResetCursorPos();
    void OnEvent(EventType, std::function<void()>);
    const double& GetDeltaTime() { return deltaTime_; }
    GLFWwindow* const GetWindow() { return window_; }
};

extern GameWindow game;
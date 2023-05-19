#pragma once

#include <opengl.h>
#include <spdlog/spdlog.h>
#include <iostream>
#include <cstdint>
#include <memory>
#include <map>
#define _USE_MATH_DEFINES
#include <math.h>
#include <unordered_map>
#include <vector>

#include "event.h"
#include "graphics/renderer.h"

#define BASE_WIDTH 1280
#define BASE_HEIGHT 720

class GameWindow {
private:
    GLFWwindow* window_ = nullptr;
    std::string title_;
    double lastMouseX_, lastMouseY_;
    bool isFullscreen_ = false;
    glm::ivec2 prevWndPos_;
    glm::ivec2 prevWndSize_;
    glm::ivec2 baseWndSize_;
    bool useVsync_;
    std::multimap<EventType, std::function<void()>> events_;

    void DispatchEvent(EventType);
public:
    GameWindow() { }
    GameWindow(const std::string&, int, int, bool = true);
    
    bool Create(Renderer&);
    void Update();
    void SetupInputSystem();
    void UpdateInputSystem();
    void ResetCursorPos();
    void OnEvent(EventType, std::function<void()>);
    bool IsUsingVsync() { return useVsync_; }
    GLFWwindow* const GetWindow() { return window_; }
};
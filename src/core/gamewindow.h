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

#include "core/event.h"
#include "core/graphics/renderer.h"

class GameWindow {
private:
    GLFWwindow* window_ = nullptr;
    std::string title_;
    int baseWidth_, baseHeight_;
    bool isFullscreen_ = false;
    glm::tvec2<int> prevWndPos_;
    glm::tvec2<int> prevWndSize_;
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
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

#include "subscriptionevent.h"
#include "graphics/renderer.h"

inline constexpr int BASE_WIDTH = 1280;
inline constexpr int BASE_HEIGHT = 720;

class GameWindow {
private:
    GLFWwindow* window_ = nullptr;
    std::string title_;
    glm::vec2 prevCursorPos_ = glm::vec2(0.0f);
    bool isFullscreen_ = false;
    bool lockMouse_ = true;
    glm::ivec2 prevWndPos_;
    glm::ivec2 prevWndSize_;
    glm::ivec2 baseWndSize_;
    bool useVsync_ = true;
    std::multimap<EventType, std::function<void()>> events_;
public:
    GameWindow() = default;
    GameWindow(const std::string&, int, int, bool = true);
    
    bool Create(Renderer&);
    void Update();
    void SetupInputSystem();
    void UpdateInputSystem();
    void ResetCursorPos();
    void DispatchEvent(EventType);
    void OnEvent(EventType, std::function<void()>);
    void ClearEvents();
    bool IsUsingVsync() { return useVsync_; }
    void UseVsync(bool);
    void LockMouse(bool);
    GLFWwindow* const GetWindow() { return window_; }
};
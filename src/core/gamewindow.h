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

#include "eventhandler.h"
#include "graphics/renderer.h"

inline constexpr int BASE_WIDTH = 1280;
inline constexpr int BASE_HEIGHT = 720;

enum class WindowEvent {
    MOUSE_MOVE,
    WINDOW_RESIZE
};
class GameWindow {
private:
    GLFWwindow* window_ = nullptr;
    std::string title_;
    glm::vec2 prevCursorPos_ = glm::vec2(0.0f);
    glm::vec2 currentMousePos_ = glm::vec2(0.0f);
    glm::vec2 relativeMousePos_ = glm::vec2(0.0f);
    bool isFullscreen_ = false;
    bool lockMouse_ = true;
    glm::ivec2 prevWndPos_;
    glm::ivec2 prevWndSize_;
    glm::ivec2 baseWndSize_;
    bool useVsync_ = true;
public:
    EventHandler<WindowEvent> eventHandler;
    GameWindow() = default;
    GameWindow(const std::string&, int, int, bool = true);
    
    bool Create(Renderer&);
    void Update();
    void SetupInputSystem();
    void UpdateInputSystem();
    void ResetCursorPos();
    bool IsUsingVsync() { return useVsync_; }
    void UseVsync(bool);
    void LockMouse(bool);
    const glm::vec2& GetMousePosition();
    // returns mouse position in window bounds in range (0, 0), (720, 1280)
    const glm::vec2& GetRelativeMousePosition();
    GLFWwindow* const GetWindow() { return window_; }
};
#include <mutex>
#include <unordered_map>

namespace Input {
    inline std::mutex keysMutex_;
    inline std::unordered_map<int, bool> keys_;
    inline std::unordered_map<int, bool> keysPressedBeforePoll_;
    inline std::unordered_map<int, bool> keysPressed_;

    inline std::mutex mouseButtonsMutex_;
    inline std::unordered_map<int, bool> mouseButtons_;
    inline std::unordered_map<int, bool> mouseButtonsPressedBeforePoll_;
    inline std::unordered_map<int, bool> mouseButtonsPressed_;

    inline std::atomic_bool WINDOW_SIZE_CHANGE_PENDING(false);
    inline std::atomic_bool SET_FULLSCREEN_PENDING(false);
    inline std::atomic_bool CURSOR_MODE_CHANGE_PENDING(false);
    inline std::atomic_bool MOUSE_MOVE_PENDING(false);
    inline std::atomic_bool FIRST_MOUSE(true);
    inline std::atomic_bool IS_MOUSE_LOCKED(false);
    inline std::atomic_bool VSYNC_POLL_RATE_CHANGE_PENDING(false);
    inline std::atomic_bool WINDOW_FOCUS_PENDING(false);
    inline std::atomic_bool CONSOLE_FOCUS_PENDING(false);
    inline bool UPDATE_FULLSCREEN = false;

    inline double MOUSE_MOVE_X;
    inline double MOUSE_MOVE_Y;

    void KeyDown(int);
    void KeyUp(int);
    bool IsKeyDown(int);
    bool IsKeyPressedDown(int);
    void ClearKeysPressedDown();
    void PollKeysPressedDown();

    void MouseButtonDown(int);
    void MouseButtonUp(int);
    bool IsMouseButtonDown(int);
    bool IsMouseButtonPressedDown(int);
    void ClearMouseButtonsPressedDown();
    void PollMouseButtonsPressedDown();
}
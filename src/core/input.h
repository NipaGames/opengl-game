#include <mutex>
#include <unordered_map>

namespace Input {
    inline std::mutex keysMutex_;
    inline std::unordered_map<int, bool> keys_;
    inline std::unordered_map<int, bool> keysPressedBeforePoll_;
    inline std::unordered_map<int, bool> keysPressed_;

    inline std::atomic_bool WINDOW_SIZE_CHANGE_PENDING(false);
    inline std::atomic_bool SET_FULLSCREEN_PENDING(false);
    inline std::atomic_bool CURSOR_MODE_CHANGE_PENDING(false);
    inline std::atomic_bool MOUSE_MOVE_PENDING(false);
    inline std::atomic_bool FIRST_MOUSE(true);
    inline std::atomic_bool IS_MOUSE_LOCKED(false);
    inline std::atomic_bool VSYNC_POLL_RATE_CHANGE_PENDING(false);
    inline bool UPDATE_FULLSCREEN = false;

    inline double MOUSE_MOVE_X;
    inline double MOUSE_MOVE_Y;

    void KeyDown(int key);
    void KeyUp(int key);
    bool IsKeyDown(int key);
    bool IsKeyPressedDown(int key);
    void ClearKeysPressedDown();
    void PollKeysPressedDown();
}
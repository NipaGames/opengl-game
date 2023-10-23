#define GLFW_DLL

#include <chrono>
#include <spdlog/spdlog.h>
#include "core/game.h"
#include "game/game.h"

int main() {
    spdlog::set_pattern(SPDLOG_PATTERN);
    #ifdef DEBUG_BUILD
        spdlog::set_level(spdlog::level::debug);
    #endif
    srand(static_cast<unsigned int>(time(0)));    
    GAME = std::make_unique<MonkeyGame>();
    if(!GAME->InitWindow())
        return EXIT_FAILURE;
    GAME->Run();
    return EXIT_SUCCESS;
}
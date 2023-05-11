#define GLFW_DLL

#include <chrono>
#include <spdlog/spdlog.h>
#include "core/game.h"
#include "game/game.h"

int main() {
    spdlog::set_pattern("[%T] %-10l %v");
    #ifdef _DEBUG
        spdlog::set_level(spdlog::level::debug);
    #endif
    srand(static_cast<unsigned int>(time(0)));    
    game = std::make_unique<MonkeyGame>();
    if(!game->InitWindow())
        return EXIT_FAILURE;
    game->Run();
    return EXIT_SUCCESS;
}
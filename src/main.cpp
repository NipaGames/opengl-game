#define GLFW_DLL

#include <chrono>
#include <spdlog/spdlog.h>
#include "core/game.h"
#include "game/game.h"

int main() {
    spdlog::set_pattern("[%T] %-10l %v");
    srand(static_cast<unsigned int>(std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())));
    game = std::make_unique<MonkeyGame>();
    if(!game->Init())
        return EXIT_FAILURE;

    game->Run();
    return EXIT_SUCCESS;
}
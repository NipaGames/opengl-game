#define GLFW_DLL

#include <chrono>
#include <spdlog/spdlog.h>
#include "gamewindow.h"

GameWindow game;

int main() {
    spdlog::set_pattern("[%T] %-10l %v");
    srand(std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()));
    game = GameWindow("peli", 1280, 720);
    if(!game.Create())
        return EXIT_FAILURE;

    game.Run();
    return EXIT_SUCCESS;
}
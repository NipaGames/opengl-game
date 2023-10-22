#pragma once

#include <core/game.h>

#define SPDLOG_PATTERN "[%T] %-10l %v"

class MonkeyGame : public Game {
public:
    bool InitWindow() override;
    void PreLoad();
    void Start();
    void Update();
};
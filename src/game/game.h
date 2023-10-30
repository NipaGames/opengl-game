#pragma once

#include <core/game.h>
#include "console.h"
#include "hud.h"

#define SPDLOG_PATTERN "[%T] %-10l %v"

class MonkeyGame : public Game {
public:
    Console console;
    HUD hud;

    bool InitWindow() override;
    void PreLoad();
    void Start();
    void Update();
    
    static MonkeyGame* GetGame();
};
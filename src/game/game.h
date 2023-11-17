#pragma once

#include <core/game.h>
#include <core/graphics/postprocessing.h>
#include "console.h"
#include "hud.h"
#include "configs.h"

#define SPDLOG_PATTERN "[%T] %-10l %v"

class MonkeyGame : public Game {
public:
    Console console;
    HUD hud;
    PostProcessing postProcessing;
    Config::Controls controlsConfig;
    Config::General generalConfig;

    bool InitWindow() override;
    void PreLoad();
    void Start();
    void Update();
    
    static MonkeyGame* GetGame();
};
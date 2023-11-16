#pragma once

#include <core/game.h>
#include <core/graphics/postprocessing.h>
#include "console.h"
#include "hud.h"
#include "controls.h"

#define SPDLOG_PATTERN "[%T] %-10l %v"

class MonkeyGame : public Game {
public:
    Console console;
    HUD hud;
    PostProcessing postProcessing;
    Controls controls;

    bool InitWindow() override;
    void PreLoad();
    void Start();
    void Update();
    
    static MonkeyGame* GetGame();
};
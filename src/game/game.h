#pragma once

#include <core/game.h>
#include <core/graphics/postprocessing.h>
#include "console.h"
#include "hud.h"
#include "configs.h"
#include "components/livingentity.h"

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
    Entity& GetPlayer();
    bool TryHitEntity(const btVector3&, const btVector3&, std::function<void(LivingEntity*)>);
    
    static MonkeyGame* GetGame();
};
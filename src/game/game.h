#pragma once

#include <core/game.h>
#include <core/graphics/postprocessing.h>
#include "configs.h"
#include "components/livingentity.h"
#include "console/console.h"
#include "ui/hud.h"
#include "ui/mainmenu.h"

#include <queue>
#include <functional>

#define SPDLOG_PATTERN "[%T] %-10l %v"

class MonkeyGame : public Game {
private:
    std::queue<std::function<void()>> eventsNextUpdate_;
    std::string nextStage_ = "teststage";
public:
    Console console;
    HUD hud;
    MainMenu mainMenu;
    std::vector<CanvasLayout*> activeUIs;
    PostProcessing postProcessing;
    Config::Controls controlsConfig;
    Config::General generalConfig;

    static void LoadStage(std::string);
    static void UnloadStage(std::string);
    static void ShowAreaMessage();
    static void SpawnPlayer();
    static void KillPlayer();

    bool InitWindow() override;
    void PreLoad();
    void Start();
    void Update();
    void ShowMainMenu();
    void SetupGame();
    void StartGame();
    void UpdateUI();
    Entity& GetPlayer();
    bool TryHitEntity(const btVector3&, const btVector3&, std::function<void(LivingEntity*)>);
    void SetNextStage(const std::string&);
    void RequestEventNextUpdate(const std::function<void()>&);
    
    static MonkeyGame* GetGame();
};
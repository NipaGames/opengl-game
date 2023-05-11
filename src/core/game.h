#pragma once

#include <memory>

#include "core/gamewindow.h"
#include "core/graphics/renderer.h"
#include "core/entity/entitymanager.h"
#define IS_MAIN_CALLED 0;

class Game {
protected:
    EntityManager entityManager_;
    GameWindow window_;
    Renderer renderer_;
    
    bool running_ = false;
    double lastFrame_;
    double deltaTime_;
public:
    virtual bool InitWindow();
    virtual void Run();
    virtual void GameThread();
    
    // Called before the window is shown
    virtual void PreLoad() { }
    // Called before the first update
    virtual void Start() { }
    // Called every frame
    virtual void Update() { }

    virtual const double& GetDeltaTime() { return deltaTime_; }
    virtual EntityManager& GetEntityManager() { return entityManager_; }
    virtual GameWindow& GetGameWindow() { return window_; }
    virtual Renderer& GetRenderer() { return renderer_; }
};

inline std::unique_ptr<Game> game;
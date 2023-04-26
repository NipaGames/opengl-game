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
    virtual bool Init();
    virtual void Run();
    virtual void GameThread();
    virtual void Update() { }
    virtual void Start() { }
    virtual const double& GetDeltaTime() { return deltaTime_; }
    virtual EntityManager& GetEntityManager() { return entityManager_; }
    virtual GameWindow& GetGameWindow() { return window_; }
    virtual Renderer& GetRenderer() { return renderer_; }
};

inline std::unique_ptr<Game> game;
#pragma once

#include <memory>

#include "gamewindow.h"
#include "graphics/renderer.h"
#include "entity/entitymanager.h"

#ifdef VERSION_MAJ
#ifdef VERSION_MIN
#define VERSION_SPECIFIED
#endif
#endif

class Game {
protected:
    EntityManager entityManager_;
    GameWindow window_;
    Renderer renderer_;
    
    bool running_ = false;
    double prevUpdate_;
    double prevFixedUpdate_;
    // time in seconds since the last frame
    double deltaTime_;
    // 0 for unlimited
    int limitFps_ = 0;
    // fixed updates per second
    int fixedUpdateRate_ = 60;
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

    virtual double GetDeltaTime() { return deltaTime_; }
    virtual double GetFixedDeltaTime() { return std::max(1.0 / (double) fixedUpdateRate_, deltaTime_); }
    virtual EntityManager& GetEntityManager() { return entityManager_; }
    virtual GameWindow& GetGameWindow() { return window_; }
    virtual Renderer& GetRenderer() { return renderer_; }
};

inline std::unique_ptr<Game> game;
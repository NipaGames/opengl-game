#pragma once

#include <memory>

#include "gamewindow.h"
#include "graphics/renderer.h"
#include "entity/entitymanager.h"
#include "io/resourcemanager.h"

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
    bool isFixedUpdate_ = false;
    // 0 for unlimited
    int limitFps_ = 0;
    // fixed updates per second
    int fixedUpdateRate_ = 60;
    bool freezeDeltaTime_ = false;
public:
    ResourceManager resources;
    virtual bool InitWindow();
    virtual void Run();
    virtual void GameThread();
    
    void GameThreadInit();
    void GameThreadStart();
    void GameThreadUpdate();
    void GameThreadCleanUp();
    void StartEntities();
    void Quit();
    // Called before the window is shown
    virtual void PreLoad() { }
    // Called before the first update
    virtual void Start() { }
    // Called every frame
    virtual void Update() { }
    virtual void FixedUpdate() { }
    virtual void CleanUp() { }

    virtual double GetDeltaTime() { return deltaTime_; }
    virtual double GetFixedDeltaTime() { return 1.0 / fixedUpdateRate_; }
    virtual void FreezeDeltaTime() { freezeDeltaTime_ = true; }
    virtual bool IsCurrentlyFixedUpdate() { return isFixedUpdate_; }
    virtual EntityManager& GetEntityManager() { return entityManager_; }
    virtual GameWindow& GetGameWindow() { return window_; }
    virtual Renderer& GetRenderer() { return renderer_; }
};

inline std::unique_ptr<Game> GAME;
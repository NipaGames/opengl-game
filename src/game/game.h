#pragma once

#include "core/game.h"

class MonkeyGame : public Game {
private:
    double lastTime_ = 0;
    int frames_;
public:
    bool Init() override;
    void Start();
    void Update();
};
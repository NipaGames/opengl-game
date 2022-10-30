#pragma once

#include "core/game.h"

class TutorialGame : public Game {
private:
    double lastTime_ = 0;
public:
    void Start();
    void Update();
};
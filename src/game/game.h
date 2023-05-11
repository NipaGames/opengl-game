#pragma once

#include "core/game.h"

class MonkeyGame : public Game {
public:
    bool InitWindow() override;
    void PreLoad();
    void Start();
    void Update();
};
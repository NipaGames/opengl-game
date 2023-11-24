#include "killplane.h"
#include "player.h"
#include <game/game.h>

void VerticalKillPlane::FixedUpdate() {
    Entity& player = MonkeyGame::GetGame()->GetPlayer();

    if (player.transform->position.y < parent->transform->position.y) {
        if (isLava) {
            MonkeyGame::GetGame()->postProcessing.gamma = 2.0f;
            MonkeyGame::GetGame()->GetRenderer().ApplyPostProcessing(MonkeyGame::GetGame()->postProcessing);
        }
        player.GetComponent<Player>()->Die();
    }
}
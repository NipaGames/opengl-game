#include "livingentity.h"

void LivingEntity::SetHealth(int health) {
    health_ = health;
}

int LivingEntity::GetHealth() const {
    return health_;
}

void LivingEntity::SetMaxHealth(int health) {
    maxHealth_ = health;
}

int LivingEntity::GetMaxHealth() const {
    return maxHealth_;
}
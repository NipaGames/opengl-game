#include "livingentity.h"

#include <opengl.h>

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

void LivingEntity::AddHealth(int health) {
    SetHealth(health_ + health);
}


void LivingEntity::Update() {
    for (int i = 0; i < statuses_.size(); i++) {
        const std::shared_ptr<StatusEffect>& status = statuses_.at(i);
        status->Update(this);
        if (status->GetTimeRemaining() < 0.0f) {
            RemoveStatus(status);
            i--;
        }
    }
}

void LivingEntity::AddStatus(const std::shared_ptr<StatusEffect>& status) {
    statuses_.push_back(status);
    status->Start(this);
}

void LivingEntity::RemoveStatus(const std::shared_ptr<StatusEffect>& status) {
    statuses_.erase(std::remove(statuses_.begin(), statuses_.end(), status), statuses_.end());
    status->End(this);
}

void StatusEffect::Start(LivingEntity*) {
    startTime_ = -time_;
    lastTick_ = startTime_;
}

float StatusEffect::GetTimeRemaining() const {
    if (time_ > 0.0f)
        return startTime_ + time_ - (float) glfwGetTime();
    else
        return 99999.0f;
}

void StatusEffect::Update(LivingEntity* entity) {
    float now = (float) glfwGetTime();
    if (now - lastTick_ >= tickrate_) {
        Tick(entity);
        lastTick_ = now;
        if (startTime_ < 0.0f) {
            startTime_ = now;
            lastTick_ = startTime_;
        }
    }
}

void PoisonEffect::Tick(LivingEntity* entity) {
    if (entity->GetHealth() > 1)
        entity->AddHealth(-1);
}
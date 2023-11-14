#include "livingentity.h"

#include <opengl.h>

void LivingEntity::SetHealth(int health) {
    health_ = health;
    CheckHealth();
}

int LivingEntity::GetHealth() const {
    return health_;
}

void LivingEntity::CheckHealth() {
    if (health_ <= 0) {
        health_ = 0;
        Die();
    }
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
        if (statusesActive_)
            status->Update(this);
        if (status->IsFinished() || (!statusesActive_ && status->GetTimeRemaining() < 0.0f)) {
            RemoveStatus(status);
            i--;
        }
    }
}

void LivingEntity::Die() {
    statusesActive_ = false;
}

void LivingEntity::AddStatus(const std::shared_ptr<StatusEffect>& status) {
    statuses_.push_back(status);
    status->Start(this);
}

void LivingEntity::RemoveStatus(const std::shared_ptr<StatusEffect>& status) {
    statuses_.erase(std::remove(statuses_.begin(), statuses_.end(), status), statuses_.end());
    status->End(this);
}

void StatusEffect::Start(LivingEntity* entity) {
    startTime_ = (float) glfwGetTime();
    lastTick_ = startTime_;
    ticks_ = (int) std::floor(time_ / tickrate_);
    ticksCounted_ = 0;
}

float StatusEffect::GetTimeRemaining() const {
    if (time_ > 0.0f)
        return startTime_ + time_ - (float) glfwGetTime();
    else
        return 99999.0f;
}

bool StatusEffect::IsFinished() const {
    return GetTimeRemaining() < 0.0f && ticksCounted_ >= ticks_;
}

void StatusEffect::Update(LivingEntity* entity) {
    if ((float) glfwGetTime() - lastTick_ >= tickrate_ && ticksCounted_ < ticks_) {
        Tick(entity);
        ticksCounted_++;
        lastTick_ = (float) glfwGetTime();
    }
}

void PoisonEffect::Tick(LivingEntity* entity) {
    if (entity->GetHealth() > 1 || !stopAt1HP)
        entity->AddHealth(-1);
}
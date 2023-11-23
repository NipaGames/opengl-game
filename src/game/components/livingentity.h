#pragma once

#include <core/entity/component.h>

class StatusEffect;
class LivingEntity : public Component<LivingEntity> {
protected:
    int health_;
    int maxHealth_;
    std::vector<std::shared_ptr<StatusEffect>> statuses_;
    bool statusesActive_ = true;
    bool deathAnimation_ = false;
    float deathAnimationStart_;
public:
    DEFINE_COMPONENT_DATA_VALUE(bool, animateMesh, true);
    DEFINE_COMPONENT_DATA_VALUE(bool, destroyWhenDead, true);
    DEFINE_COMPONENT_DATA_VALUE(float, deathAnimationLength, .5f);
    DEFINE_COMPONENT_DATA_VALUE(int, health, 100);
    DEFINE_COMPONENT_DATA_VALUE(int, maxHealth, 100);
    virtual void TryDestroy();
    virtual void SetHealth(int);
    virtual int GetHealth() const;
    virtual void CheckHealth();
    virtual void Die();
    virtual void SetMaxHealth(int);
    virtual int GetMaxHealth() const;
    virtual void AddHealth(int);
    virtual void TakeDamage(int);
    virtual void AddStatus(const std::shared_ptr<StatusEffect>&);
    virtual void RemoveStatus(const std::shared_ptr<StatusEffect>&);
    virtual void Update() override;
    virtual void Start() override;
    template<typename T>
    void AddStatusType(float t = 0.0f) {
        std::shared_ptr<StatusEffect> status = std::make_shared<T>(t);
        for (const auto& s : statuses_) {
            if (s->GetName() == status->GetName())
                return;
        }
        AddStatus(status);
    }
};

class StatusEffect {
protected:
    const std::string name_;
    float tickrate_;
    float lastTick_ = 0.0f;
    float startTime_;
    float time_;
    int ticks_ = 0;
    int ticksCounted_;
public:
    StatusEffect(const std::string& n, float rate, float t) : name_(n), tickrate_(rate), time_(t) { }
    virtual void Tick(LivingEntity*) { }
    virtual void Start(LivingEntity*);
    virtual void End(LivingEntity*) { }
    float GetTimeRemaining() const;
    bool IsFinished() const;
    void Update(LivingEntity*);
    const std::string& GetName() const { return name_; }
};

enum class StatusEffectType {
    POISON,
    RADIATION
};

class PoisonEffect : public StatusEffect {
protected:
    bool stopAt1HP = true;
public:
    PoisonEffect(float t) : StatusEffect("poison", 1.0f, t) { }
    PoisonEffect(const std::string& n, float rate, float t) : StatusEffect(n, rate, t) { }
    virtual void Tick(LivingEntity*);
};

class RadiationEffect : public PoisonEffect {
public:
    RadiationEffect(float t) : PoisonEffect("radiation", .1f, t) { stopAt1HP = false; }
};
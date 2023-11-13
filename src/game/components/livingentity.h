#pragma once

#include <core/entity/component.h>

class LivingEntity : public Component<LivingEntity> {
protected:
    int health_ = 100;
    int maxHealth_ = 100;
public:
    virtual void SetHealth(int);
    virtual int GetHealth() const;
    virtual void SetMaxHealth(int);
    virtual int GetMaxHealth() const;
};
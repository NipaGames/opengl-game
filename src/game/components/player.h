#pragma once

#include "livingentity.h"

#include <opengl.h>
#include <core/entity/component.h>
#include <core/physics/component/rigidbody.h>
#include <core/entity/serializable.h>
#include <core/eventhandler.h>

#include <BulletDynamics/Character/btKinematicCharacterController.h>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>

enum class PlayerEvent {
    ATTACK,
    ATTACK_ANIMATION_COMPLETE,
    GAME_OVER
};

enum class BufferActionType {
    NONE,
    ATTACK
};

class PlayerController : public Component<PlayerController> {
private:
    void OnMouseMove();
    bool isFlying_ = false;
    btKinematicCharacterController* characterController_ = nullptr;
    btPairCachingGhostObject* ghostObject_ = nullptr;
    float stepHeight_ = 0.25f;
    float gravity_ = -9.8f * 3.0f;
    float jumpSpeed_ = 10.0f;
    bool pushRigidBodies_ = false;
    bool running_ = false;
    bool gameOver_ = false;
    float controlSpeedModifier_ = 1.0f;
    float speedModifierTime_ = .5f;
    float baseSensitivity_ = .2f;
    bool isOnGround_ = false;
    bool isMoving_ = false;
    bool isInInputMode_ = false;
    float attackCooldown_ = 1.0f;
    float attackStart_ = -attackCooldown_;
    float attackRange_ = 2.5f;
    // see dark souls input buffering
    BufferActionType actionBuffer_ = BufferActionType::NONE;
    // buffering time (seconds)
    float bufferOnset_ = 0.25f;

    void CheckForAttacks();
    void ClearBuffer();
public:
    EventHandler<PlayerEvent> eventHandler;
    float sensitivity;
    glm::vec2 mouseMove = glm::vec2(0.0f);

    ~PlayerController();
    void Spawn();
    void Start();
    void Update();
    void ActivateGameOverState();
    bool IsMoving() { return isMoving_; }
    bool IsOnGround() { return isOnGround_; }
    bool IsInInputMode() { return isInInputMode_; }
    bool IsGameOver() { return gameOver_; }

    SDEF(float, speed) = SVAL(10.0f);
    SDEF(glm::vec3, spawnPosition) = SVAL(glm::vec3(0.0f));
    SDEFVD(int, testNums);
};

class Player : public LivingEntity {
private:
    void UpdateHUD();
    int xp_ = 0;
public:
    void Start();
    void SetHealth(int) override;
    void SetMaxHealth(int) override;
    void Die() override;
    void AddStatus(const std::shared_ptr<StatusEffect>&) override;
    void RemoveStatus(const std::shared_ptr<StatusEffect>&) override;
    void AddXP(int);
};
REGISTER_COMPONENT(Player);
#pragma once

#include "livingentity.h"

#include <opengl.h>
#include <core/entity/component.h>
#include <core/physics/component/rigidbody.h>
#include <core/entity/serializable.h>
#include <BulletDynamics/Character/btKinematicCharacterController.h>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>

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
public:
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
public:
    void Start();
    void SetHealth(int) override;
    void SetMaxHealth(int) override;
    void Die() override;
    void AddStatus(const std::shared_ptr<StatusEffect>&) override;
    void RemoveStatus(const std::shared_ptr<StatusEffect>&) override;
};
REGISTER_COMPONENT(Player);
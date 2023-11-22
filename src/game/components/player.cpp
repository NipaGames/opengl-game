#include "player.h"

#include <game/game.h>

#include <core/game.h>
#include <core/input.h>
#include <core/physics/utils.h>
#include <core/graphics/postprocessing.h>

PlayerController::~PlayerController() {
    delete characterController_;
    delete ghostObject_;
}

void PlayerController::OnMouseMove() {
    auto& cam = GAME->GetRenderer().GetCamera();
    float f = sensitivity * baseSensitivity_ * controlSpeedModifier_;
    mouseMove = glm::vec2(Input::MOUSE_MOVE_X, Input::MOUSE_MOVE_Y) * f;
    cam.yaw   += mouseMove.x;
    cam.pitch += mouseMove.y;

    if(cam.pitch > 89.0f)
        cam.pitch = 89.0f;
    if(cam.pitch < -89.0f)
        cam.pitch = -89.0f;
}

void PlayerController::Spawn() {
    characterController_->reset(Physics::dynamicsWorld);
	characterController_->warp(Physics::GLMVectorToBtVector3(spawnPosition));
}

void PlayerController::Start() {
    sensitivity = MonkeyGame::GetGame()->controlsConfig.sensitivity;
    controlSpeedModifier_ = 1.0f;
    ghostObject_ = new btPairCachingGhostObject();
    ghostObject_->setWorldTransform(parent->transform->btGetTransform());
    Physics::axisSweep->getOverlappingPairCache()->setInternalGhostPairCallback(new btGhostPairCallback());
    btConvexShape* capsuleCollider = new btCapsuleShape(.5f, 2.0f);
    ghostObject_->setCollisionShape(capsuleCollider);
	ghostObject_->setCollisionFlags(btCollisionObject::CF_CHARACTER_OBJECT);
    ghostObject_->setCollisionFlags(ghostObject_->getCollisionFlags() | btCollisionObject::CF_DISABLE_VISUALIZE_OBJECT);
    characterController_ = new btKinematicCharacterController(ghostObject_, capsuleCollider, stepHeight_, btVector3(0.0f, 1.0f, 0.0f));
    characterController_->setMaxPenetrationDepth(stepHeight_);
    characterController_->setGravity(btVector3(0.0f, gravity_, 0.0f));
    characterController_->setJumpSpeed(btScalar(jumpSpeed_));
    Physics::dynamicsWorld->addCollisionObject(ghostObject_, btBroadphaseProxy::CharacterFilter, btBroadphaseProxy::StaticFilter | btBroadphaseProxy::DefaultFilter);
    GAME->GetGameWindow().eventHandler.Subscribe(WindowEvent::MOUSE_MOVE, [this]() { 
        this->OnMouseMove();
    });
    const std::vector<std::string> msgs = { "pow", "ouch", "crunch", "crick", "crack" };
    eventHandler.Subscribe(PlayerEvent::ATTACK_ANIMATION_COMPLETE, [msgs]() {
        std::cout << msgs[rand() % msgs.size()] << std::endl;
    });
    Spawn();
}

void PlayerController::Update() {
    auto& cam = GAME->GetRenderer().GetCamera();
    glm::vec3 front = cam.front;
    front.y = 0.0f;
    front = glm::normalize(front);

    btVector3 rayFrom(parent->transform->position.x, parent->transform->position.y, parent->transform->position.z);
    btVector3 rayTo(rayFrom.getX(), rayFrom.getY() - 1.5f, rayFrom.getZ());
    btCollisionWorld::ClosestRayResultCallback res(rayFrom, rayTo);
    Physics::dynamicsWorld->rayTest(rayFrom, rayTo, res);
    isOnGround_ = res.hasHit() || characterController_->onGround();

    if (gameOver_) {
        controlSpeedModifier_ -= (float) GAME->GetDeltaTime() / speedModifierTime_;
        controlSpeedModifier_ = std::max(0.0f, controlSpeedModifier_);
    }

    glm::vec3 velocity(0.0f);
    isMoving_ = false;
    isInInputMode_ = Input::IS_MOUSE_LOCKED;
    if (isInInputMode_) {
        if (Input::IsMouseButtonPressedDown(GLFW_MOUSE_BUTTON_1) && glfwGetTime() - attackStart_ > attackCooldown_) {
            eventHandler.Dispatch(PlayerEvent::ATTACK);
            attackStart_ = (float) glfwGetTime();
        }
        if (Input::IsKeyDown(GLFW_KEY_W))
            velocity += front;
        if (Input::IsKeyDown(GLFW_KEY_S))
            velocity -= front;
        if (Input::IsKeyDown(GLFW_KEY_A))
            velocity -= glm::normalize(glm::cross(front, cam.up));
        if (Input::IsKeyDown(GLFW_KEY_D))
            velocity += glm::normalize(glm::cross(front, cam.up));
        
        if (velocity != glm::vec3(0.0f)) {
            isMoving_ = true;
        }

        if (Input::IsKeyPressedDown(GLFW_KEY_LEFT_SHIFT))
            running_ = !running_;
            
        if (velocity != glm::vec3(0.0f))
            velocity = glm::normalize(velocity);
        if (!gameOver_) {
            if (Input::IsKeyPressedDown(GLFW_KEY_F)) {
                isFlying_ = !isFlying_;
                if (isFlying_) {
                    characterController_->setGravity(btVector3(0.0f, 0.0f, 0.0f));
                    characterController_->setLinearVelocity(btVector3(0.0f, 0.0f, 0.0f));
                    ghostObject_->setCollisionFlags(ghostObject_->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
                }
                else {
                    characterController_->setGravity(btVector3(0.0f, gravity_, 0.0f));
                    ghostObject_->setCollisionFlags(ghostObject_->getCollisionFlags() & ~btCollisionObject::CF_NO_CONTACT_RESPONSE);
                }
            }
            if (isOnGround_) {
                characterController_->setStepHeight(stepHeight_);
                characterController_->setMaxPenetrationDepth(stepHeight_);
            }
            else {
                characterController_->setStepHeight(0.0f);
                characterController_->setMaxPenetrationDepth(0.05f);
            }
            if (Input::IsKeyPressedDown(GLFW_KEY_SPACE) && isOnGround_ && !isFlying_) {
                characterController_->jump();
            }
        }

        if (isFlying_) {
            if (Input::IsKeyDown(GLFW_KEY_SPACE))
                velocity.y += 1.0f;
            if (Input::IsKeyDown(GLFW_KEY_LEFT_CONTROL))
                velocity.y -= 1.0f;
        }
    }
    float movingSpeed = speed;
    if (running_) {
        movingSpeed *= 2.0f;
    }
    velocity *= movingSpeed;
    characterController_->setWalkDirection(btVector3(velocity.x, velocity.y, velocity.z) * controlSpeedModifier_ * btScalar(GAME->GetDeltaTime()));

    parent->transform->btSetTransform(ghostObject_->getWorldTransform());
    characterController_->updateAction(Physics::dynamicsWorld, btScalar(GAME->GetDeltaTime()));

    cam.pos.x = parent->transform->position.x;
    cam.pos.z = parent->transform->position.z;
    cam.pos.y = parent->transform->position.y + 1.0f;
}

void PlayerController::ActivateGameOverState() {
    eventHandler.Dispatch(PlayerEvent::GAME_OVER);
    gameOver_ = true;
    controlSpeedModifier_ = 1.0f;
}

void Player::Start() {
    maxHealth_ = 100;
    health_ = maxHealth_;
    UpdateHUD();
}

void Player::UpdateHUD() {
    HUD& hud = MonkeyGame::GetGame()->hud;
    hud.UpdateHP(health_, maxHealth_);
}

void Player::SetHealth(int health) {
    LivingEntity::SetHealth(health);
    UpdateHUD();
}

void Player::SetMaxHealth(int health) {
    LivingEntity::SetMaxHealth(health);
    UpdateHUD();
}

void Player::Die() {
    LivingEntity::Die();
    parent->GetComponent<PlayerController>()->ActivateGameOverState();
    MonkeyGame::GetGame()->hud.GameOver();
}

void Player::AddStatus(const std::shared_ptr<StatusEffect>& status) {
    if (!statusesActive_)
        return;
    LivingEntity::AddStatus(status);
    MonkeyGame::GetGame()->hud.AddStatus(status->GetName());
}

void Player::RemoveStatus(const std::shared_ptr<StatusEffect>& status) {
    MonkeyGame::GetGame()->hud.RemoveStatus(status->GetName());
    LivingEntity::RemoveStatus(status);
}
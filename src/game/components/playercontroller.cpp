#include "playercontroller.h"

#include <core/event.h>
#include <core/game.h>
#include <core/input.h>

PlayerController::~PlayerController() {
    delete characterController_;
    delete ghostObject_;
}

void PlayerController::OnMouseMove() {
    float sensitivity = 0.1f;
    auto& cam = game->GetRenderer().GetCamera();

    cam.yaw   += static_cast<float>(Input::MOUSE_MOVE_X) * sensitivity;
    cam.pitch += static_cast<float>(Input::MOUSE_MOVE_Y) * sensitivity;

    if(cam.pitch > 89.0f)
        cam.pitch = 89.0f;
    if(cam.pitch < -89.0f)
        cam.pitch = -89.0f;
}

void PlayerController::Spawn() {
    characterController_->reset(Physics::dynamicsWorld);
	characterController_->warp(btVector3(spawnPosition.x, spawnPosition.y, spawnPosition.z));
}

void PlayerController::Start() {
    ghostObject_ = new btPairCachingGhostObject();
    ghostObject_->setWorldTransform(parent->transform->btGetTransform());
    Physics::axisSweep->getOverlappingPairCache()->setInternalGhostPairCallback(new btGhostPairCallback());
    btConvexShape* capsuleCollider = new btCapsuleShape(.5f, 2.0f);
    ghostObject_->setCollisionShape(capsuleCollider);
	ghostObject_->setCollisionFlags(btCollisionObject::CF_CHARACTER_OBJECT);
    ghostObject_->setCollisionFlags(ghostObject_->getCollisionFlags() | btCollisionObject::CF_DISABLE_VISUALIZE_OBJECT);
    characterController_ = new btKinematicCharacterController(ghostObject_, capsuleCollider, stepHeight_, btVector3(0.0f, 1.0f, 0.0f));
    characterController_->setMaxPenetrationDepth(.2f);
    Physics::dynamicsWorld->addCollisionObject(ghostObject_, btBroadphaseProxy::CharacterFilter, btBroadphaseProxy::StaticFilter | btBroadphaseProxy::DefaultFilter);
    Physics::dynamicsWorld->addAction(characterController_);
    game->GetGameWindow().OnEvent(EventType::MOUSE_MOVE, [this]() { 
        this->OnMouseMove();
    });
    Spawn();
}

void PlayerController::Update() {
    auto& cam = game->GetRenderer().GetCamera();
    glm::vec3 front = cam.front;
    front.y = 0.0f;
    front = glm::normalize(front);

    glm::vec3 velocity(0.0f);
    if (Input::IS_MOUSE_LOCKED) {
        if (Input::IsKeyDown(GLFW_KEY_W))
            velocity += front;
        if (Input::IsKeyDown(GLFW_KEY_S))
            velocity -= front;
        if (Input::IsKeyDown(GLFW_KEY_A))
            velocity -= glm::normalize(glm::cross(front, cam.up));
        if (Input::IsKeyDown(GLFW_KEY_D))
            velocity += glm::normalize(glm::cross(front, cam.up));
            
        if (velocity != glm::vec3(0.0f))
            velocity = glm::normalize(velocity);

        if (Input::IsKeyPressedDown(GLFW_KEY_F)) {
            isFlying_ = !isFlying_;
        }
        if (Input::IsKeyPressedDown(GLFW_KEY_SPACE) && characterController_->canJump()) {
            characterController_->jump();
        }
    }
    velocity *= speed;
    characterController_->setWalkDirection(btVector3(velocity.x, velocity.y, velocity.z));
    parent->transform->btSetTransform(ghostObject_->getWorldTransform());
    
    cam.pos.x = parent->transform->position.x;
    cam.pos.z = parent->transform->position.z;

    cam.pos.y = parent->transform->position.y + 1.0f;
}
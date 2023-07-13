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
    characterController_->setGravity(btVector3(0.0f, gravity_, 0.0f));
    characterController_->setJumpSpeed(btScalar(jumpSpeed_));
    Physics::dynamicsWorld->addCollisionObject(ghostObject_, btBroadphaseProxy::CharacterFilter, btBroadphaseProxy::StaticFilter | btBroadphaseProxy::DefaultFilter);
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

    btVector3 rayFrom(parent->transform->position.x, parent->transform->position.y, parent->transform->position.z);
    btVector3 rayTo(rayFrom.getX(), rayFrom.getY() - 2.0f, rayFrom.getZ());
    btCollisionWorld::ClosestRayResultCallback res(rayFrom, rayTo);
    Physics::dynamicsWorld->rayTest(rayFrom, rayTo, res);
    bool isOnGround = res.hasHit() || characterController_->onGround();

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
        if (Input::IsKeyPressedDown(GLFW_KEY_SPACE) && isOnGround && !isFlying_) {
            characterController_->jump();
        }

        if (isFlying_) {
            if (Input::IsKeyDown(GLFW_KEY_SPACE))
                velocity.y += 1.0f;
            if (Input::IsKeyDown(GLFW_KEY_LEFT_SHIFT))
                velocity.y -= 1.0f;
        }
    }
    velocity *= speed;
    characterController_->setWalkDirection(btVector3(velocity.x, velocity.y, velocity.z) * btScalar(game->GetDeltaTime()));

    parent->transform->btSetTransform(ghostObject_->getWorldTransform());
    if (pushRigidBodies_) {
        if (glm::length(velocity) > 0.0f) {
            for (int i = 0; i < ghostObject_->getNumOverlappingObjects(); i++) {
                btCollisionObject* obj = ghostObject_->getOverlappingObject(i);
                btRigidBody* rb = static_cast<btRigidBody*>(obj);
                if (rb == nullptr)
                    continue;
                if (rb->getMass() == 0.0f)
                    continue;
                rb->setLinearVelocity(btVector3(velocity.x, rb->getLinearVelocity().getY(), velocity.z));
                rb->applyCentralForce(btVector3(velocity.x, velocity.y, velocity.z));
            }
        }
    }

    characterController_->updateAction(Physics::dynamicsWorld, btScalar(game->GetDeltaTime()));

    cam.pos.x = parent->transform->position.x;
    cam.pos.z = parent->transform->position.z;

    cam.pos.y = parent->transform->position.y + 1.0f;
}
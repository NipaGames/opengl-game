#include "playercontroller.h"

#include <core/event.h>
#include <core/game.h>
#include <core/input.h>

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
    parent->transform->position = spawnPosition;
}

void PlayerController::Start() {
    rigidBody_ = parent->GetComponent<Physics::RigidBody>();
    if (rigidBody_ == nullptr) {
        rigidBody_ = parent->AddComponent<Physics::RigidBody>();
        rigidBody_->collider = new btCapsuleShape(.5f, 2.0f);
        rigidBody_->EnableDebugVisualization(false);
        rigidBody_->EnableRotation(false);
        rigidBody_->interpolate = true;
        rigidBody_->doesMassAffectGravity = true;
        rigidBody_->mass = 2.0f;
    }
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

    glm::vec3 dPos = (parent->transform->position - prevPos) / (float) game->GetDeltaTime();
    prevPos = parent->transform->position;

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
        else
            movementSpeed_ = 0.0f;

        if (Input::IsKeyPressedDown(GLFW_KEY_F)) {
            isFlying_ = !isFlying_;
        }
        movementSpeed_ += (float) game->GetDeltaTime() * speed * accelerationSpeed_;
        movementSpeed_ = std::min(movementSpeed_, speed);
        velocity *= movementSpeed_;
        btVector3 v = rigidBody_->rigidBody->getLinearVelocity();
        v.setX(velocity.x);
        v.setZ(velocity.z);
        bool grounded = rigidBody_->IsGrounded(2.0f);
        if (Input::IsKeyPressedDown(GLFW_KEY_SPACE) && grounded)
            v.setY(8);
        if (!grounded && glm::length(glm::vec2(v.getX(), v.getZ())) > 5.0f && (abs(velocity.x) > abs(dPos.x) * 100.0f || abs(velocity.z) * game->GetDeltaTime() > abs(dPos.z) * 100.0f)) {
            wallHugTimer_ += game->GetDeltaTime();
            if (wallHugTimer_ >= .025f) {
                v = btVector3(0.0f, v.getY(), 0.0f);
            }
        }
        else if (wallHugTimer_ != 0.0f) {
            wallHugTimer_ = 0.0f;
        }
        rigidBody_->rigidBody->setLinearVelocity(v);
        if (isFlying_) {
            if (Input::IsKeyDown(GLFW_KEY_SPACE))
                parent->transform->position.y += speed * static_cast<float>(game->GetDeltaTime());
            if (Input::IsKeyDown(GLFW_KEY_LEFT_SHIFT))
                parent->transform->position.y -= speed * static_cast<float>(game->GetDeltaTime());
        }
    }

    rigidBody_->rigidBody->activate(true);

    if (!isFlying_ && parent->transform->position.y <= 0.0f && velocity.y < 0.0f) {
        velocity.y = 0;
        parent->transform->position.y = 0.0f;
    }

    cam.pos.x = parent->transform->position.x;
    cam.pos.z = parent->transform->position.z;

    cam.pos.y = parent->transform->position.y + 1.0f;
}
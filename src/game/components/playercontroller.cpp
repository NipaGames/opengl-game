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

    if (Input::IS_MOUSE_LOCKED) {
        glm::vec3 velocity(0.0f);
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
        velocity *= speed;
        btVector3 v = rigidBody_->rigidBody->getLinearVelocity();
        v.setX(velocity.x);
        v.setZ(velocity.z);
        if (Input::IsKeyPressedDown(GLFW_KEY_SPACE) && rigidBody_->IsGrounded(2.0f))
            v.setY(5);
        
        rigidBody_->rigidBody->setLinearVelocity(v);
        if (isFlying_) {
            if (Input::IsKeyDown(GLFW_KEY_SPACE))
                parent->transform->position.y += speed * static_cast<float>(game->GetDeltaTime());
            if (Input::IsKeyDown(GLFW_KEY_LEFT_SHIFT))
                parent->transform->position.y -= speed * static_cast<float>(game->GetDeltaTime());
         }
    }

    if (!isFlying_ && parent->transform->position.y <= 0.0f && velocity.y < 0.0f) {
        velocity.y = 0;
        parent->transform->position.y = 0.0f;
    }

    // whacky ahh way to make falling a bit more effective
    if (rigidBody_->IsGrounded(2.0f))
        rigidBody_->rigidBody->setLinearFactor(btVector3(1.0f, 0.5f, 1.0f));
    else if (rigidBody_->rigidBody->getLinearVelocity().getY() < 0.0f)
        rigidBody_->rigidBody->setLinearFactor(btVector3(1.0f, 1.5f, 1.0f));
    else
        rigidBody_->rigidBody->setLinearFactor(btVector3(1.0f, 1.0f, 1.0f));
    rigidBody_->rigidBody->activate(true);

    cam.pos.x = parent->transform->position.x;
    cam.pos.z = parent->transform->position.z;

    cam.pos.y = parent->transform->position.y + 1.0f;
}
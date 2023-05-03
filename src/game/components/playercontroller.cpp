#include "game/components/playercontroller.h"

#include "core/event.h"
#include "core/game.h"
#include "core/input.h"

#define GRAVITY 9.81f
#define MAX_FALL_VELOCITY -53

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
    game->GetGameWindow().OnEvent(EventType::MOUSE_MOVE, [this]() { 
        this->OnMouseMove();
    });
    Spawn();
}

void PlayerController::Update() {
    auto& cam = game->GetRenderer().GetCamera();
    const float moveSpeed = speed * static_cast<float>(game->GetDeltaTime());
    glm::vec3 front = cam.front;
    front.y = 0.0f;
    front = glm::normalize(front);

    if (Input::IS_MOUSE_LOCKED) {
        glm::vec3 horizontalVelocity(0.0f);
        if (Input::IsKeyDown(GLFW_KEY_W))
            horizontalVelocity += front;
        if (Input::IsKeyDown(GLFW_KEY_S))
            horizontalVelocity -= front;
        if (Input::IsKeyDown(GLFW_KEY_A))
            horizontalVelocity -= glm::normalize(glm::cross(front, cam.up));
        if (Input::IsKeyDown(GLFW_KEY_D))
            horizontalVelocity += glm::normalize(glm::cross(front, cam.up));
            
        if (horizontalVelocity != glm::vec3(0.0f))
            horizontalVelocity = glm::normalize(horizontalVelocity);

        if (Input::IsKeyPressedDown(GLFW_KEY_F)) {
            isFlying_ = !isFlying_;
        }

        parent->transform->position += horizontalVelocity * moveSpeed;
        if (Input::IsKeyPressedDown(GLFW_KEY_SPACE) && parent->transform->position.y < .1)
            velocity.y = 10;
        
         if (isFlying_) {
            if (Input::IsKeyDown(GLFW_KEY_SPACE))
                parent->transform->position.y += speed * static_cast<float>(game->GetDeltaTime());
            if (Input::IsKeyDown(GLFW_KEY_LEFT_SHIFT))
                parent->transform->position.y -= speed * static_cast<float>(game->GetDeltaTime());
         }
    }

    if (velocity.y > MAX_FALL_VELOCITY)
        velocity.y -= GRAVITY * mass * static_cast<float>(game->GetDeltaTime());

    if (!isFlying_ && parent->transform->position.y <= 0.0f && velocity.y < 0.0f) {
        velocity.y = 0;
        parent->transform->position.y = 0.0f;
    }

    if (!isFlying_)
        parent->transform->position += velocity * static_cast<float>(game->GetDeltaTime());
    else
        velocity = glm::vec3(0.0f);

    cam.pos.x = parent->transform->position.x;
    cam.pos.z = parent->transform->position.z;

    cam.pos.y = parent->transform->position.y + 2.0f;
}
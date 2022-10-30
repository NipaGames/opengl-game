#include "game/components/playercontroller.h"

#include "core/event.h"
#include "core/game.h"
#include "core/input.h"

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

void PlayerController::Start() {
    game->GetGameWindow().OnEvent(EventType::MOUSE_MOVE, [this]() { 
        this->OnMouseMove();
    });
    parent->transform->position.z = -10.0f;
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

        parent->transform->position += horizontalVelocity * moveSpeed;
        if (Input::IsKeyDown(GLFW_KEY_SPACE))
            parent->transform->position += moveSpeed * cam.up;
        if (Input::IsKeyDown(GLFW_KEY_LEFT_SHIFT))
            parent->transform->position -= moveSpeed * cam.up;
        cam.pos = parent->transform->position;
    }
}
#include "entity/component/playercontroller.h"

#include "event.h"
#include "gamewindow.h"
#include "input.h"

void PlayerController::OnMouseMove() {
    if (!Input::IS_MOUSE_LOCKED)
        return;

    double xPos, yPos;
    glfwGetCursorPos(game.GetWindow(), &xPos, &yPos);

    if (Input::FIRST_MOUSE) {
        lastMouseX_ = xPos;
        lastMouseY_ = yPos;
        Input::FIRST_MOUSE = false;
    }

    float offsetX = xPos - lastMouseX_;
    float offsetY = lastMouseY_ - yPos; 

    lastMouseX_ = xPos;
    lastMouseY_ = yPos;

    float sensitivity = 0.1f;
    offsetX *= sensitivity;
    offsetY *= sensitivity;

    auto& cam = game.renderer.GetCamera();

    cam.yaw   += offsetX;
    cam.pitch += offsetY;

    if(cam.pitch > 89.0f)
        cam.pitch = 89.0f;
    if(cam.pitch < -89.0f)
        cam.pitch = -89.0f;
}

void PlayerController::Start() {
    game.OnEvent(EventType::MOUSE_MOVE, [this]() { 
        this->OnMouseMove();
    });
    parent->transform->position.z = -10.0f;
}

void PlayerController::Update() {
    auto& cam = game.renderer.GetCamera();
    const float moveSpeed = speed * game.GetDeltaTime();
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
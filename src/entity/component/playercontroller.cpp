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

    Camera& cam = game.renderer.GetCamera();

    cam.yaw   += offsetX;
    cam.pitch += offsetY;

    if(cam.pitch > 89.0f)
        cam.pitch = 89.0f;
    if(cam.pitch < -89.0f)
        cam.pitch = -89.0f;

    glm::vec3 direction;
    direction.x = cos(glm::radians(cam.yaw)) * cos(glm::radians(cam.pitch));
    direction.y = sin(glm::radians(cam.pitch));
    direction.z = sin(glm::radians(cam.yaw)) * cos(glm::radians(cam.pitch));
    cam.front = glm::normalize(direction);
}

void PlayerController::Start() {
    game.OnEvent(EventType::MOUSE_MOVE, [this]() { 
        this->OnMouseMove();
    });
}

void PlayerController::Update() {

}
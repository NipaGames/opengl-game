#include "huditem.h"

#include <core/entity/entity.h>
#include <game/game.h>

void HUDItemRenderer::SetupShaders() {
    SHADER_LIT_ = Shader("HUD_ITEM_LIT");
    SHADER_UNLIT_ = Shader("HUD_ITEM_UNLIT");

    SHADER_LIT_.Use();
    SHADER_LIT_.SetUniform("hudView", glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
    SHADER_UNLIT_.Use();
    SHADER_UNLIT_.SetUniform("hudView", glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -1.0f)));

    glUseProgram(0);
}

void HUDItemRenderer::Start() {
    if (!SHADERS_DEFINED_) {
        SetupShaders();
        SHADERS_DEFINED_ = true;
    }
    alwaysOnFrustum = true;
    MeshRenderer::Start();
}

const Shader& HUDItemRenderer::GetMaterialShader(const std::shared_ptr<Material>& mat) const {
    const Shader& shader = mat->GetShader();
    if (shader.GetIDString() == "LIT") {
        return SHADER_LIT_;
    }
    else if (shader.GetIDString() == "UNLIT") {
        return SHADER_UNLIT_;
    }
    return shader;
}

void ItemInHand::OnMouseMove() {
    if (player_->IsInInputMode()) {
        horizontalMovementTarget_ -= player_->mouseMove.x * horizontalMovementSpeed_;
        prevMouseMove_ = (float) glfwGetTime();
        horizontalMovementTarget_ = std::min(horizontalMovementTarget_, horizontalMovement_);
        horizontalMovementTarget_ = std::max(horizontalMovementTarget_, -horizontalMovement_);
    }
}

void ItemInHand::Start() {
    player_ = MonkeyGame::GetGame()->GetPlayer().GetComponent<PlayerController>();
    itemStartPos_ = glm::vec2(parent->transform->position.x, parent->transform->position.y);
    GAME->GetGameWindow().OnEvent(EventType::MOUSE_MOVE, [this]() { 
        this->OnMouseMove();
    });
}

void ItemInHand::Update() {
    bool bob = player_->IsMoving() && player_->IsOnGround();
    if (bob || std::abs(parent->transform->position.y - itemStartPos_.y) > .0001f) {
        if (bob) {
            bobbingPos_ += (float) GAME->GetDeltaTime();
        }
        else {
            // check if the cosine slope is increasing with the derivative
            bool hasCompletedHalf = -std::sinf((float) M_PI * bobbingSpeed_ * bobbingPos_) > 0.0f;
            if (hasCompletedHalf) {
                bobbingPos_ += (float) GAME->GetDeltaTime();
            }
            else {
                bobbingPos_ -= (float) GAME->GetDeltaTime();
            }
        }
        parent->transform->position.y = itemStartPos_.y + (std::cosf((float) M_PI * bobbingSpeed_ * bobbingPos_) / 2.0f - .5f) * bobbingAmount_;
    }
    else {
        bobbingPos_ = 0.0f;
    }

    float horizontalSpeed = 1.0f;
    if ((float) glfwGetTime() - prevMouseMove_ > .1f) {
        horizontalSpeed = 2.0f;
        horizontalMovementTarget_ = 0.0f;
    }
    float targetPos = itemStartPos_.x + horizontalMovementTarget_;
    if (std::abs(parent->transform->position.x - targetPos) > .0001f) {
        parent->transform->position.x -= (parent->transform->position.x - targetPos) * (float) GAME->GetDeltaTime() * horizontalSpeed;
    }
}
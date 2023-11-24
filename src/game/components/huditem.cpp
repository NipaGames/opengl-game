#include "huditem.h"

#include <core/entity/entity.h>
#include <game/game.h>
#include <core/graphics/component/light.h>

void HUDItemRenderer::SetupShaders() {
    SHADER_LIT = Shader("HUD_ITEM_LIT");
    SHADER_UNLIT = Shader("HUD_ITEM_UNLIT");

    SHADER_LIT.Use();
    SHADER_LIT.SetUniform("hudView", glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
    SHADER_UNLIT.Use();
    SHADER_UNLIT.SetUniform("hudView", glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -1.0f)));

    glUseProgram(0);
}

void HUDItemRenderer::Start() {
    if (!SHADERS_DEFINED) {
        SetupShaders();
        SHADERS_DEFINED = true;
    }
    alwaysOnFrustum = true;
    MeshRenderer::Start();
}

const Shader& HUDItemRenderer::GetMaterialShader(const std::shared_ptr<Material>& mat) const {
    const Shader& shader = mat->GetShader();
    if (shader.GetIDString() == "LIT") {
        return SHADER_LIT;
    }
    else if (shader.GetIDString() == "UNLIT") {
        return SHADER_UNLIT;
    }
    return shader;
}

void ItemInHand::OnMouseMove() {
    if (player_->IsInInputMode()) {
        if (std::abs(player_->mouseMove.x) > .1f) {
            horizontalMovementTarget_ -= player_->mouseMove.x * horizontalMovementSpeed_;
            prevMouseMove_ = (float) glfwGetTime();
            horizontalMovementTarget_ = std::min(horizontalMovementTarget_, horizontalMovement_);
            horizontalMovementTarget_ = std::max(horizontalMovementTarget_, -horizontalMovement_);
        }
    }
}

void ItemInHand::FixPosition() {
    int w, h;
    glfwGetWindowSize(GAME->GetGameWindow().GetWindow(), &w, &h);
    fixedItemStartPos_ = itemStartPos_;
    fixedItemStartPos_.x *= (9.0f * w) / (16.0f * h);
}

const int HUD_LIGHT_INDEX = 31;
const std::string HUD_LIGHT_NAME = "lights[" + std::to_string(HUD_LIGHT_INDEX) + "]";

glm::vec3 GetHUDLightDir() {
    return -GAME->GetRenderer().GetCamera().front;
}

void ItemInHand::Start() {
    player_ = MonkeyGame::GetGame()->GetPlayer().GetComponent<PlayerController>();
    itemStartPos_ = parent->transform->position;
    
    Lights::ReserveIndex(HUD_LIGHT_INDEX);
    HUDItemRenderer::SHADER_LIT.Use();
    HUDItemRenderer::SHADER_LIT.SetUniform(std::string(HUD_LIGHT_NAME + ".type").c_str(), static_cast<int>(Lights::LightType::DIRECTIONAL));
    HUDItemRenderer::SHADER_LIT.SetUniform(std::string(HUD_LIGHT_NAME + ".enabled").c_str(), true);
    HUDItemRenderer::SHADER_LIT.SetUniform(std::string(HUD_LIGHT_NAME + ".intensity").c_str(), .1f);
    HUDItemRenderer::SHADER_LIT.SetUniform(std::string(HUD_LIGHT_NAME + ".color").c_str(), glm::vec3(1.0f));
    HUDItemRenderer::SHADER_LIT.SetUniform(std::string(HUD_LIGHT_NAME + ".dir").c_str(), GetHUDLightDir());
    HUDItemRenderer::SHADER_LIT.SetUniform(std::string(HUD_LIGHT_NAME + ".overrideNormals").c_str(), true);
    HUDItemRenderer::SHADER_LIT.SetUniform(std::string(HUD_LIGHT_NAME + ".normal").c_str(), glm::vec3(1.0f, 1.0f, 0.0f));
    glUseProgram(0);
    
    GAME->GetGameWindow().eventHandler.Subscribe(WindowEvent::MOUSE_MOVE, [this]() { 
        this->OnMouseMove();
    });
    GAME->GetGameWindow().eventHandler.Subscribe(WindowEvent::WINDOW_RESIZE, [this]() { 
        this->FixPosition();
    });
    player_->eventHandler.Subscribe(PlayerEvent::GAME_OVER, [this]() {
        this->BringToBackground();
    });
    player_->eventHandler.Subscribe(PlayerEvent::ATTACK, [this]() {
        this->AttackAnimation();
    });
    FixPosition();
}

void ItemInHand::FixedUpdate() {
    // optimally the lights position would be relative to the hud item position instead of world space
    // however, that would require heavy fragment shader reprogramming and would suck
    HUDItemRenderer::SHADER_LIT.Use();
    HUDItemRenderer::SHADER_LIT.SetUniform(std::string(HUD_LIGHT_NAME + ".dir").c_str(), GetHUDLightDir());
    glUseProgram(0);
}

void ItemInHand::BringToBackground() {
    parent->GetComponent<HUDItemRenderer>()->renderAfterPostProcessing = false;
    parent->GetComponent<HUDItemRenderer>()->renderLate = true;
}

void ItemInHand::AttackAnimation() {
    attackTime_ = (float) glfwGetTime();
    isAttacking_ = true;
    isReturningFromAttack_ = false;
    attackStartPos_ = parent->transform->position;
}

void ItemInHand::Update() {
    if (isAttacking_ || isReturningFromAttack_) {
        float attackPos = (float) glfwGetTime() - attackTime_;
        if (isAttacking_) {
            attackPos /= attackAnimationLength_;
            attackPos *= attackPos;
            if (attackPos >= 1.0f) {
                attackPos = 1.0f;
                isAttacking_ = false;
                isReturningFromAttack_ = true;
                attackTime_ = (float) glfwGetTime();
                player_->eventHandler.Dispatch(PlayerEvent::ATTACK_ANIMATION_COMPLETE);
            }
        }
        else if (isReturningFromAttack_) {
            attackPos /= attackAnimationReturnLength_;
            attackPos = 1.0f - attackPos;
            if (attackPos <= 0.0f) {
                attackPos = 0.0f;
                isReturningFromAttack_ = false;
            }
        }
        parent->transform->position = attackStartPos_ + attackPos * attackMove_;
        parent->transform->rotation = attackPos * glm::radians(attackRotation_);
    }
    else {
        bool bob = player_->IsMoving() && player_->IsOnGround();
        if (bob || std::abs(parent->transform->position.y - fixedItemStartPos_.y) > .0001f) {
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
            parent->transform->position.y = fixedItemStartPos_.y + (std::cosf((float) M_PI * bobbingSpeed_ * bobbingPos_) / 2.0f - .5f) * bobbingAmount_;
        }
        else {
            bobbingPos_ = 0.0f;
        }

        float horizontalSpeed = 1.0f;
        if ((float) glfwGetTime() - prevMouseMove_ > .1f) {
            horizontalSpeed = 2.0f;
            horizontalMovementTarget_ = 0.0f;
        }
        float targetPos = fixedItemStartPos_.x + horizontalMovementTarget_;
        if (std::abs(parent->transform->position.x - targetPos) > .0001f) {
            parent->transform->position.x -= (parent->transform->position.x - targetPos) * (float) GAME->GetDeltaTime() * horizontalSpeed;
        }

    }
}
#pragma once

#include <core/graphics/component/meshrenderer.h>
#include "player.h"

class HUDItemRenderer : public MeshRenderer {
public:
    inline static Shader SHADER_LIT;
    inline static Shader SHADER_UNLIT;
    inline static bool SHADERS_DEFINED = false;

    static void SetupShaders();
    void Start() override;
    const Shader& GetMaterialShader(const std::shared_ptr<Material>&) const override;
};
REGISTER_COMPONENT(HUDItemRenderer);

class ItemInHand : public Component<ItemInHand> {
private:
    PlayerController* player_;
    glm::vec3 itemStartPos_, fixedItemStartPos_;

    float bobbingPos_;
    float bobbingAmount_ = .015f;
    float bobbingSpeed_ = 3.0f;
    float bobbingReturnSpeed_ = 15.0f;

    float prevMouseMove_ = 0.0f;
    float horizontalMovement_ = .15f;
    float horizontalMovementSpeed_ = .001f;
    float horizontalMovementTarget_ = 0.0f;

    float attackTime_ = 0.0f;
    float attackAnimationLength_ = .3f;
    float attackAnimationReturnLength_ = .7f;
    glm::vec3 attackStartPos_;
    glm::vec3 attackMove_ = glm::vec3(-.1f, -.05f, -.2f);
    glm::vec3 attackRotation_ = glm::vec3(-15.0f, 5.0f, 2.5f);
    bool isAttacking_ = false;
    bool isReturningFromAttack_ = false;

    void OnMouseMove();
    void FixPosition();
    void AttackAnimation();
    void BringToBackground();
public:
    void Start() override;
    void Update() override;
    void FixedUpdate() override;
};
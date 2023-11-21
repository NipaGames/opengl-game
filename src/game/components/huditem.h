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
    glm::vec2 itemStartPos_;

    float bobbingPos_;
    float bobbingAmount_ = .015f;
    float bobbingSpeed_ = 3.0f;
    float bobbingReturnSpeed_ = 15.0f;

    float prevMouseMove_ = 0.0f;
    float horizontalMovement_ = .15f;
    float horizontalMovementSpeed_ = .001f;
    float horizontalMovementTarget_ = 0.0f;

    bool hasGameOverStateActivated_ = false;

    void OnMouseMove();
public:
    void OnGameOver();
    void Start() override;
    void Update() override;
    void FixedUpdate() override;
};
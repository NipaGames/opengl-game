#pragma once

#include <core/graphics/component/meshrenderer.h>
#include "player.h"

class HUDItemRenderer : public MeshRenderer {
private:
    inline static Shader SHADER_LIT_;
    inline static Shader SHADER_UNLIT_;
    inline static bool SHADERS_DEFINED_ = false;
public:
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

    void OnMouseMove();
public:
    void Start() override;
    void Update() override;
};
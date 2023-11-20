#pragma once

#include <core/graphics/component/meshrenderer.h>

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
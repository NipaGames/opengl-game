#include "huditem.h"

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
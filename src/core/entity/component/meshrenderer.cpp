#include "core/entity/component/meshrenderer.h"

#include "core/entity/entity.h"
#include "core/game.h"

void MeshRenderer::Start() {
    game->GetRenderer().AddMeshRenderer(this);
}

void MeshRenderer::Render(const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix, const Shader* shader) {
    bool useDefaultShader = true;
    if (shader != nullptr)
        useDefaultShader = false;
    
    for (auto mesh : meshes) {
        if (useDefaultShader)
            shader = &mesh->material->GetShader();
        // default uniforms
        // idk if it would be better to define these as default initializers for Material class
        shader->Use();
        shader->SetUniform<glm::vec3>("material.color", glm::vec3(1.0f));

        mesh->material->Use();
        mesh->Bind();
        
        glm::mat4 modelTransform(1.0f);
        modelTransform = glm::translate(modelTransform, parent->transform->position);
        modelTransform *= glm::toMat4(parent->transform->rotation);
        modelTransform = glm::scale(modelTransform, parent->transform->size);

        shader->SetUniform<glm::mat4>("projection", projectionMatrix);
        shader->SetUniform<glm::mat4>("view", viewMatrix);
        shader->SetUniform<glm::mat4>("model", modelTransform);
        shader->SetUniform<glm::vec3>("viewPos", game->GetRenderer().GetCamera().pos);
        mesh->Render();

        // unbinding
        glBindVertexArray(0);
    }
}
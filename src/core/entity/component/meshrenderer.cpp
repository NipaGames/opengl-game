#include "core/entity/component/meshrenderer.h"

#include "core/entity/entity.h"
#include "core/game.h"

void MeshRenderer::Start() {
    game->GetRenderer().AddMeshRenderer(this);
}

void MeshRenderer::Render(const glm::mat4& camMatrix) {
    for (auto mesh : meshes) {
        // default uniforms
        // idk if it would be better to define these as default initializers for Material class
        mesh->material->GetShader().Use();
        mesh->material->GetShader().SetUniform<glm::vec3>("material.color", glm::vec3(1.0f));

        mesh->material->Use();
        mesh->Bind();
        
        glm::mat4 modelTransform(1.0f);
        modelTransform = glm::translate(modelTransform, parent->transform->position);
        modelTransform *= glm::toMat4(parent->transform->rotation);
        modelTransform = glm::scale(modelTransform, parent->transform->size);
 
        mesh->material->GetShader().SetUniform<glm::mat4>("viewProjection", camMatrix);
        mesh->material->GetShader().SetUniform<glm::mat4>("model", modelTransform);
        mesh->material->GetShader().SetUniform<glm::vec3>("viewPos", game->GetRenderer().GetCamera().pos);
        mesh->Render();

        // unbinding
        glBindVertexArray(0);
    }
}
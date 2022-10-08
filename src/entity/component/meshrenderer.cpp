#include "entity/component/meshrenderer.h"
#include "gamewindow.h"

void MeshRenderer::Start() {    
    game.renderer.AddMeshRenderer(this);
}

void MeshRenderer::Render(const glm::mat4& camMatrix) {
    for (auto mesh : meshes) {
        mesh->material->Use();
        glBindVertexArray(mesh->vao);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ebo);

        glm::mat4 modelTransform(1.0f);
        modelTransform = glm::translate(modelTransform, parent->transform->position);
        modelTransform *= glm::toMat4(parent->transform->rotation);
        modelTransform = glm::scale(modelTransform, parent->transform->size);
 
        mesh->material->GetShader().SetUniform<glm::mat4>("viewProjection", camMatrix);
        mesh->material->GetShader().SetUniform<glm::mat4>("model", modelTransform);
        mesh->material->GetShader().SetUniform<glm::vec3>("viewPos", game.renderer.GetCamera().pos);
        mesh->Render();

        //Unbinding
        glBindVertexArray(0);
    }
}
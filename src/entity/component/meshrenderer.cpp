#include "entity/component/meshrenderer.h"
#include "gamewindow.h"

void MeshRenderer::Start() {    
    game.renderer.AddMeshRenderer(this);
}

void MeshRenderer::Render(const glm::mat4& camMatrix) {
    for (auto mesh : meshes) {
        mesh->material.Use();
        glBindVertexArray(mesh->vao);

        glBindBuffer(GL_ARRAY_BUFFER, colorBuffer_);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
        glEnableVertexAttribArray(1);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ebo);

        glm::mat4 transform(1.0f);
        transform = glm::translate(transform, parent->transform->position);
        transform *= glm::toMat4(parent->transform->rotation);
        transform = glm::scale(transform, parent->transform->size);
 
        glm::mat4 mvpMatrix = camMatrix * transform;
        mesh->material.GetShader().SetUniform<glm::mat4>("transform", mvpMatrix);
        mesh->Render();
    }
}
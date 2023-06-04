#include "meshrenderer.h"

#include <core/entity/entity.h>
#include <core/game.h>
#include <core/io/serializetypes.h>

MeshRenderer::~MeshRenderer() {
    if (isAdded)
        game->GetRenderer().RemoveMeshRenderer(this);
}

void MeshRenderer::Start() {
    if (!isAdded)
        game->GetRenderer().AddMeshRenderer(this);
}

void MeshRenderer::CalculateModelMatrix() {
    modelMatrix_ = glm::mat4(1.0f);
    modelMatrix_ = glm::translate(modelMatrix_, parent->transform->position);
    modelMatrix_ *= glm::toMat4(parent->transform->rotation);
    modelMatrix_ = glm::scale(modelMatrix_, parent->transform->size);
}

void MeshRenderer::Render(const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix, const Shader* shader) const {
    bool useDefaultShader = true;
    if (shader != nullptr)
        useDefaultShader = false;
    
    for (auto mesh : meshes) {
        if (mesh->material == nullptr)
            continue;
        if (useDefaultShader)
            shader = &mesh->material->GetShader();
        // default uniforms
        // idk if it would be better to define these as default initializers for Material class
        shader->Use();
        shader->SetUniform<glm::vec3>("material.color", glm::vec3(1.0f));

        mesh->material->Use();
        mesh->Bind();

        shader->SetUniform("projection", projectionMatrix);
        shader->SetUniform("view", viewMatrix);
        shader->SetUniform("model", modelMatrix_);
        shader->SetUniform("viewPos", game->GetRenderer().GetCamera().pos);
        mesh->Render();

        // unbinding
        glBindVertexArray(0);
    }
}

bool MeshRenderer::IsOnFrustum(const ViewFrustum& frustum) const {
    glm::vec3 center = parent->transform->position;

    // Scaled orientation
    glm::vec3 right = modelMatrix_[0] * .5f;
    glm::vec3 up = modelMatrix_[1] * .5f;
    glm::vec3 forward = -modelMatrix_[2] * .5f;

    glm::vec3 extents;

    extents.x =
        std::abs(glm::dot(glm::vec3{ 1.0f, 0.0f, 0.0f }, right)) +
        std::abs(glm::dot(glm::vec3{ 1.0f, 0.0f, 0.0f }, up)) +
        std::abs(glm::dot(glm::vec3{ 1.0f, 0.0f, 0.0f }, forward));

    extents.y =
        std::abs(glm::dot(glm::vec3{ 0.0f, 1.0f, 0.0f }, right)) +
        std::abs(glm::dot(glm::vec3{ 0.0f, 1.0f, 0.0f }, up)) +
        std::abs(glm::dot(glm::vec3{ 0.0f, 1.0f, 0.0f }, forward));

    extents.z =
        std::abs(glm::dot(glm::vec3{ 0.0f, 0.0f, 1.0f }, right)) +
        std::abs(glm::dot(glm::vec3{ 0.0f, 0.0f, 1.0f }, up)) +
        std::abs(glm::dot(glm::vec3{ 0.0f, 0.0f, 1.0f }, forward));

    //We not need to divise scale because it's based on the half extention of the AABB
    ViewFrustum::AABB newAABB { center, extents };
    return frustum.IsOnFrustum(newAABB);
}
#include "meshrenderer.h"

#include <limits>

#include <core/entity/entity.h>
#include <core/game.h>
#include <core/io/serializetypes.h>

Shader aabbShader = Shader(Shaders::ShaderID::UNLIT);

MeshRenderer::~MeshRenderer() {
    if (isAdded)
        GAME->GetRenderer().RemoveMeshRenderer(this);
}

void MeshRenderer::Start() {
    if (!object.empty()) {
        for (auto mesh : GAME->resources.modelManager[object].meshes) {
            meshes.push_back(mesh);
        }
    }
    if (isStatic)
        CalculateModelMatrix();
    if (!isAdded)
        GAME->GetRenderer().AddMeshRenderer(this);
    glm::vec3 aabbMin = glm::vec3(std::numeric_limits<float>::max());
    glm::vec3 aabbMax = glm::vec3(-std::numeric_limits<float>::max());
    for (const auto& mesh : meshes) {
        glm::vec3 meshMin = mesh->aabb.GetMin();
        glm::vec3 meshMax = mesh->aabb.GetMax();
        aabbMin = glm::vec3(std::min(aabbMin.x, meshMin.x), std::min(aabbMin.y, meshMin.y), std::min(aabbMin.z, meshMin.z));
        aabbMax = glm::vec3(std::max(aabbMax.x, meshMax.x), std::max(aabbMax.y, meshMax.y), std::max(aabbMax.z, meshMax.z));
    }
    aabb_ = ViewFrustum::AABB::FromMinMax(aabbMin, aabbMax);
    glBindVertexArray(0);
}

void MeshRenderer::CalculateModelMatrix() {
    modelMatrix_ = glm::mat4(1.0f);
    modelMatrix_ = glm::translate(modelMatrix_, parent->transform->position);
    modelMatrix_ *= glm::toMat4(parent->transform->rotation);
    modelMatrix_ = glm::scale(modelMatrix_, parent->transform->size);
}

void MeshRenderer::Render(const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix, const Shader* shader, bool aabbDebug) const {
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
        shader->SetUniform("material.color", glm::vec3(1.0f));
        shader->SetUniform("material.opacity", 1.0f);

        mesh->material->Use();
        mesh->Bind();

        shader->SetUniform("projection", projectionMatrix);
        shader->SetUniform("view", viewMatrix);
        shader->SetUniform("model", modelMatrix_);
        shader->SetUniform("viewPos", GAME->GetRenderer().GetCamera().pos);
        shader->SetUniform("time", (float) glfwGetTime());
        mesh->Render();

        // unbinding
        glBindVertexArray(0);
    }
    // not gonna optimize this since it's just a debug visualizer
    // yeah, it's pretty horrible that a new meshes gets created every frame
    // well, at least this doesn't cause a memory leak afaik (meshes are also deleted)
    if (aabbDebug) {
        glm::vec3 aabbMin = aabb_.GetMin();
        glm::vec3 aabbMax = aabb_.GetMax();
        Mesh aabbMesh(
            {
                aabbMin.x, aabbMax.y, aabbMax.z,
                aabbMin.x, aabbMin.y, aabbMax.z,
                aabbMax.x, aabbMax.y, aabbMax.z,
                aabbMax.x, aabbMin.y, aabbMax.z,
                aabbMin.x, aabbMax.y, aabbMin.z,
                aabbMin.x, aabbMin.y, aabbMin.z,
                aabbMax.x, aabbMax.y, aabbMin.z,
                aabbMax.x, aabbMin.y, aabbMin.z
            },
            {
                3, 2, 0, 1, 3, 0,
                7, 6, 2, 3, 7, 2,
                5, 4, 6, 7, 5, 6,
                1, 0, 4, 5, 1, 4,
                6, 4, 0, 2, 6, 0,
                1, 5, 7, 1, 7, 3,
            }
        );
        aabbMesh.GenerateVAO();

        aabbShader.Use();
        glBindVertexArray(aabbMesh.vao);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, aabbMesh.ebo);

        aabbShader.SetUniform("projection", projectionMatrix);
        aabbShader.SetUniform("view", viewMatrix);
        aabbShader.SetUniform("model", modelMatrix_);
        aabbShader.SetUniform("viewPos", GAME->GetRenderer().GetCamera().pos);
        aabbShader.SetUniform("time", (float) glfwGetTime());

        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        aabbShader.SetUniform("material.color", glm::vec3(1.0f, 0.0f, 0.0f));
        aabbShader.SetUniform("material.opacity", 1.0f);
        aabbMesh.Render();

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}

bool MeshRenderer::IsOnFrustum(const ViewFrustum& frustum) const {
    if (alwaysOnFrustum)
        return true;
    
    glm::vec3 center = parent->transform->position + glm::vec3(parent->transform->rotation * glm::vec4(aabb_.center * parent->transform->size, 1.0f));

    // Scaled orientation
    glm::vec3 right = modelMatrix_[0] * aabb_.extents.x;
    glm::vec3 up = modelMatrix_[1] * aabb_.extents.y;
    glm::vec3 forward = -modelMatrix_[2] * aabb_.extents.z;

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

// serialization
#include <core/io/serializer.h>
#include <core/io/serializetypes.h>

JSON_SERIALIZE_TYPES([](Serializer::SerializationArgs& args, const nlohmann::json& j) {
    auto mesh = Meshes::CreateMeshInstance(Meshes::CUBE);
    mesh->material = GAME->GetRenderer().GetMaterial("MAT_DEFAULT");
    args.Return(mesh);
    return true;
}, std::shared_ptr<Mesh>);
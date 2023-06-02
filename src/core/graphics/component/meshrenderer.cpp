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

void MeshRenderer::Render(const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix, const Shader* shader) {
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
        
        glm::mat4 modelTransform(1.0f);
        modelTransform = glm::translate(modelTransform, parent->transform->position);
        modelTransform *= glm::toMat4(parent->transform->rotation);
        modelTransform = glm::scale(modelTransform, parent->transform->size);

        shader->SetUniform("projection", projectionMatrix);
        shader->SetUniform("view", viewMatrix);
        shader->SetUniform("model", modelTransform);
        shader->SetUniform("viewPos", game->GetRenderer().GetCamera().pos);
        mesh->Render();

        // unbinding
        glBindVertexArray(0);
    }
}

// just a placeholder for now
JSON_SERIALIZE_TYPES([](Serializer::SerializationArgs& args, const nlohmann::json& j) {
    auto mesh = Meshes::CreateMeshInstance(Meshes::CUBE);
    mesh->material = std::make_shared<Material>(Shaders::ShaderID::LIT);
    args.Return(mesh);
    return true;
}, std::shared_ptr<Mesh>);
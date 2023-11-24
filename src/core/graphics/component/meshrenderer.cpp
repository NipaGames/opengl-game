#include "meshrenderer.h"

#include <limits>

#include <core/entity/entity.h>
#include <core/game.h>
#include <core/io/serializetypes.h>
#include <core/terrain/plane.h>

Shader aabbShader = Shader(Shaders::ShaderID::UNLIT);

MeshRenderer::~MeshRenderer() {
    if (isAdded)
        GAME->GetRenderer().RemoveMeshRenderer(this);
}

void MeshRenderer::Start() {
    if (!object.empty()) {
        for (const auto& mesh : GAME->resources.modelManager[object].meshes) {
            if (copyMeshes) {
                std::shared_ptr<Mesh> meshCopy = std::make_shared<Mesh>(*mesh);
                meshCopy->material = mesh->material;
                meshCopy->GenerateVAO();
                meshes.push_back(meshCopy);
            }
            else {
                meshes.push_back(mesh);
            }
        }
    }
    if (isStatic)
        CalculateModelMatrix();
    if (!isAdded) {
        GAME->GetRenderer().AddMeshRenderer(this);
    }
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

void MeshRenderer::UseMaterial(const std::shared_ptr<Material>& mat) const {
    mat->Use();
}

void MeshRenderer::UpdateUniforms(const Shader& shader, const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix, const glm::mat4& transformMatrix) const {
    shader.Use();
    // default uniforms
    // idk if it would be better to define these as default initializers for Material class
    shader.SetUniform("material.color", glm::vec3(1.0f));
    shader.SetUniform("material.opacity", 1.0f);

    shader.SetUniform("projection", projectionMatrix);
    shader.SetUniform("view", viewMatrix);
    shader.SetUniform("model", modelMatrix_ * transformMatrix);
    shader.SetUniform("viewPos", GAME->GetRenderer().GetCamera().pos);
    shader.SetUniform("time", (float) glfwGetTime());
}

const Shader& MeshRenderer::GetMaterialShader(const std::shared_ptr<Material>& mat) const {
    return mat->GetShader();
}

void MeshRenderer::Render(const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix, const Shader* shader, bool aabbDebug) const {
    if (disableDepthTest)
        glDisable(GL_DEPTH_TEST);
    
    bool useDefaultShaders = (shader == nullptr);
    for (const auto& mesh : meshes) {
        if (mesh->material == nullptr)
            continue;
        
        if (useDefaultShaders) {
            const Shader& s = GetMaterialShader(mesh->material);
            UpdateUniforms(s, projectionMatrix, viewMatrix, mesh->transformMatrix);
            mesh->material->Use(s);
            if (useCustomMaterial) {
                customMaterial.Use(s);
                s.SetUniform("material.hasTexture", mesh->material->GetTexture() != TEXTURE_NONE);
            }
        }
        else {
            UpdateUniforms(*shader, projectionMatrix, viewMatrix, mesh->transformMatrix);
            mesh->material->Use();
        }


        mesh->Bind();
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
    glBindVertexArray(0);
    if (disableDepthTest)
        glEnable(GL_DEPTH_TEST);
}

bool MeshRenderer::IsOnFrustum(const ViewFrustum& frustum) const {
    if (alwaysOnFrustum)
        return true;
    
    glm::vec3 center = modelMatrix_  * glm::vec4(aabb_.center, 1.0f);

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
    if (!j.is_object())
        return false;
    if (!j.contains("type") || !j["type"].is_string())
        return false;
    std::string type = j["type"];
    if (!j.contains("material") || !j["material"].is_string())
        return false;
    std::string material = j["material"];
    std::shared_ptr<Mesh> mesh = nullptr;

    if (type == "CUBE") {
        mesh = Meshes::CreateMeshInstance(Meshes::CUBE);
    }
    else if (type == "TERRAIN") {
        glm::vec2 tiling = glm::vec2(1.0f);
        float variation = 0.0f;

        if (j.contains("tiling")) {
            if (!Serializer::SetJSONPointerValue(&tiling, j["tiling"])) {
                return false;
            }
        }
        if (j.contains("variation")) {
            if (!Serializer::SetJSONPointerValue(&variation, j["variation"])) {
                return false;
            }
        }
        
        std::shared_ptr<Plane> plane = std::make_shared<Plane>(tiling);
        plane->heightVariation = variation;
        plane->GenerateVertices();
        mesh = plane;
    }

    if (mesh == nullptr)
        return false;
    mesh->material = GAME->GetRenderer().GetMaterial(material);
    mesh->GenerateVAO();
    args.Return(mesh);
    return true;
}, std::shared_ptr<Mesh>);
#include "model.h"

#include <spdlog/spdlog.h>
#include <core/io/resourcemanager.h>
#include <core/game.h>

std::shared_ptr<Mesh> Model::ProcessMesh(const aiMesh* mesh, const aiScene* scene) {
    auto processedMesh = std::make_shared<Mesh>();
    for(unsigned int i = 0; i < mesh->mNumVertices; i++) {
        processedMesh->vertices.push_back(mesh->mVertices[i].x);
        processedMesh->vertices.push_back(mesh->mVertices[i].y);
        processedMesh->vertices.push_back(mesh->mVertices[i].z);

        processedMesh->normals.push_back(mesh->mNormals[i].x);
        processedMesh->normals.push_back(mesh->mNormals[i].y);
        processedMesh->normals.push_back(mesh->mNormals[i].z);

        if(mesh->mTextureCoords[0]) {
            processedMesh->texCoords.push_back(mesh->mTextureCoords[0][i].x);
            processedMesh->texCoords.push_back(mesh->mTextureCoords[0][i].y);
        }
        else {
            processedMesh->texCoords.push_back(0.0f);
            processedMesh->texCoords.push_back(0.0f);

        }
    }
    for(unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for(unsigned int j = 0; j < face.mNumIndices; j++)
            processedMesh->indices.push_back(face.mIndices[j]);
    }
    glm::vec3 aabbMin(mesh->mAABB.mMin.x, mesh->mAABB.mMin.y, mesh->mAABB.mMin.z);
    glm::vec3 aabbMax(mesh->mAABB.mMax.x, mesh->mAABB.mMax.y, mesh->mAABB.mMax.z);
    processedMesh->aabb = ViewFrustum::AABB::FromMinMax(aabbMin, aabbMax);
    processedMesh->GenerateVAO();
    return processedMesh;
}

void Model::ProcessNodes(const aiNode* node, const aiScene* scene) {
    for(unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]]; 
        meshes.push_back(ProcessMesh(mesh, scene));	
    }
    for(unsigned int i = 0; i < node->mNumChildren; i++) {
        ProcessNodes(node->mChildren[i], scene);
    }
}

void Model::LoadModel(const std::string& path) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenBoundingBoxes);
    if (!scene) {
        spdlog::error("Failed loading model '" + path + "'!");
        return;
    }
    aiNode* rootNode = scene->mRootNode;
	
    if(scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !rootNode) {
        spdlog::error(importer.GetErrorString());
        return;
    }
    dir_ = path.substr(0, path.find_last_of('/'));
    ProcessNodes(rootNode, scene);
}

std::optional<Model> Resources::ModelManager::LoadResource(const std::fs::path& path) {
    Model model;
    model.LoadModel(path.string());
    std::string id = GetItemID();
    if (GAME->resources.objectsFile.GetItems().count(id) != 0) {
        const Object& objData = GAME->resources.objectsFile.GetItem(id);
        if (objData.defaultMaterial != nullptr) {
            for (auto& m : model.meshes) {
                m->material = objData.defaultMaterial;
                if (objData.size != glm::vec3(1.0f)) {
                    for (int i = 0; i < m->vertices.size(); i++) {
                        m->vertices[i] *= objData.size[i % 3];
                    }
                    m->aabb.center *= objData.size;
                    m->aabb.extents *= objData.size;
                    m->GenerateVAO();
                }
            }
        }
        for (const auto& [i, mat] : objData.materials) {
            if (i >= model.meshes.size())
                continue;
            model.meshes.at(i)->material = mat;
        }
    }
    return model;
}
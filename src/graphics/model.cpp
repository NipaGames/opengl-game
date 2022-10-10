#include "graphics/model.h"

#include <spdlog/spdlog.h>

std::shared_ptr<Mesh> Model::ProcessMesh(const aiMesh* mesh, const aiScene* scene) {
    auto processedMesh = std::make_shared<Mesh>();
    for(int i = 0; i < mesh->mNumVertices; i++) {
        processedMesh->vertices.push_back(mesh->mVertices[i].x);
        processedMesh->vertices.push_back(mesh->mVertices[i].y);
        processedMesh->vertices.push_back(mesh->mVertices[i].z);

        processedMesh->normals.push_back(mesh->mNormals[i].x);
        processedMesh->normals.push_back(mesh->mNormals[i].y);
        processedMesh->normals.push_back(mesh->mNormals[i].z);
    }
    for(int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for(unsigned int j = 0; j < face.mNumIndices; j++)
            processedMesh->indices.push_back(face.mIndices[j]);
    }
    return processedMesh;
}

void Model::ProcessNodes(const aiNode* node, const aiScene* scene) {
    for(int i = 0; i < node->mNumMeshes; i++) {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]]; 
        meshes.push_back(ProcessMesh(mesh, scene));	
    }
    for(int i = 0; i < node->mNumChildren; i++) {
        ProcessNodes(node->mChildren[i], scene);
    }
}

void Model::LoadModel(const std::string& path) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
    if (!scene) {
        spdlog::error("Failed loading model '" + path + "'!");
        return;
    }
    aiNode* rootNode = scene->mRootNode;
	
    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !rootNode) {
        spdlog::error(importer.GetErrorString());
        return;
    }
    dir_ = path.substr(0, path.find_last_of('/'));
    ProcessNodes(rootNode, scene);
}

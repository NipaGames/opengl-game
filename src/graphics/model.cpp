#include "graphics/model.h"

#include <spdlog/spdlog.h>

Mesh Model::ProcessMesh(const aiMesh* mesh, const aiScene* scene) {
    for (int i = 0; i < mesh->mNumVertices; i++) {
        
    }
    Mesh m;
    return m;
}

void Model::ProcessNodes(const aiNode* node, const aiScene* scene) {
    for(int i = 0; i < node->mNumMeshes; i++) {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]]; 
        meshes_.push_back(ProcessMesh(mesh, scene));	
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

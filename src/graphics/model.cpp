#include "graphics/model.h"

#include <spdlog/spdlog.h>

/*void Model::ProcessNodes(const aiNode* node, const aiScene* scene) {
    for(int i = 0; i < node->mNumMeshes; i++) {
        /*aiMesh *mesh = scene->mMeshes[rootNode->mMeshes[i]]; 
        meshes.push_back(processMesh(mesh, scene));/		
    }
    for(int i = 0; i < node->mNumChildren; i++) {
        //processNode(rootNode->mChildren[i], scene);
    }
}*/

void Model::LoadModel(const std::string& path) {
    /*Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
    aiNode* rootNode = scene->mRootNode;
	
    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !rootNode) 
    {
        spdlog::error(importer.GetErrorString());
        return;
    }
    dir_ = path.substr(0, path.find_last_of('/'));
    ProcessNodes(rootNode, scene);*/
}

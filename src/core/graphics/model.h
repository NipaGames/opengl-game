#pragma once

#include <iostream>
#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "core/graphics/mesh.h"

class Model {
private:
    std::string dir_;
    void ProcessNodes(const aiNode*, const aiScene*);
    std::shared_ptr<Mesh> ProcessMesh(const aiMesh*, const aiScene*);
public:
    std::vector<std::shared_ptr<Mesh>> meshes;
    void LoadModel(const std::string&);
};
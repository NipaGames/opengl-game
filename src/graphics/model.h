#pragma once

#include <iostream>
#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "graphics/mesh.h"

class Model {
private:
    std::vector<Mesh> meshes_;
    std::string dir_;
    void ProcessNodes(const aiNode*, const aiScene*);
    Mesh ProcessMesh(const aiMesh*, const aiScene*);
public:
    void LoadModel(const std::string&);
};
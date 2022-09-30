#pragma once

#include <iostream>
#include <vector>

#include "graphics/mesh.h"

class Model {
private:
    std::vector<Mesh> meshes_;
    std::string dir_;
    //void ProcessNodes(const aiNode*, const aiScene*);
public:
    void LoadModel(const std::string&);
};
#pragma once

#include <opengl.h>

#include "core/graphics/mesh.h"

class Plane : public Mesh {
private:
    glm::ivec2 tiling_;
    std::vector<std::vector<float>> heightGrid_;
public:
    Plane() : tiling_(1, 1) { }
    Plane(const glm::ivec2& t) : tiling_(t) { }
    void GenerateVertices();
};
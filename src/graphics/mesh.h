#pragma once

#include <opengl.h>
#include <memory>
#include <string>
#include <vector>

#include "graphics/material.h"
#include "graphics/shader.h"

class Mesh {
public:
    GLuint vao;
    GLuint vbo;
    GLuint ebo;
    Mesh() { }
    Mesh(const std::vector<float>& v, const std::vector<unsigned int>& i) : vertices(v), indices(i) { }
    Mesh(const std::string& meshId, const std::vector<float>& v, const std::vector<unsigned int>& i) : id(meshId), vertices(v), indices(i) { }
    Mesh(const Mesh& m) : id(m.id), vertices(m.vertices), indices(m.indices){ }
    ~Mesh();
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    std::shared_ptr<Material> material;
    std::string id;
    bool cullFaces = true;
    void GenerateVAO();
    void Render() const;
};

namespace Meshes {
std::shared_ptr<Mesh> CreateMeshInstance(const Mesh&);
static Mesh CUBE(
    "cube",
    {
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f
    },
    {
        3, 2, 0, 1, 3, 0,
        7, 6, 2, 3, 7, 2,
        5, 4, 6, 7, 5, 6,
        1, 0, 4, 5, 1, 4,
        6, 4, 0, 2, 6, 0,
        1, 5, 7, 1, 7, 3,
    });
};
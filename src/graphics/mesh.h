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
    Material material;
    void GenerateVAO();
    void Render() const;
    std::string id;
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
        0, 2, 3, 0, 3, 1,
        2, 6, 7, 2, 7, 3,
        6, 4, 5, 6, 5, 7,
        4, 0, 1, 4, 1, 5,
        0, 4, 6, 0, 6, 2,
        7, 5, 1, 3, 7, 1,
    });
};
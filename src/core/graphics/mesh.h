#pragma once

#include <opengl.h>
#include <memory>
#include <string>
#include <vector>

#include "core/graphics/material.h"
#include "core/graphics/shader.h"

class Mesh {
public:
    GLuint vao;
    GLuint vbo;
    GLuint ebo;
    GLuint normalBuffer;
    Mesh() { }
    Mesh(const std::vector<float>& v, const std::vector<unsigned int>& i, const std::vector<float>& n) : vertices(v), indices(i), normals(n) { }
    Mesh(const std::string& meshId, const std::vector<float>& v, const std::vector<unsigned int>& i, const std::vector<float>& n) : id(meshId), vertices(v), indices(i), normals(n) { }
    Mesh(const std::vector<float>& v, const std::vector<unsigned int>& i) : vertices(v), indices(i), normals(v) { }
    Mesh(const std::string& meshId, const std::vector<float>& v, const std::vector<unsigned int>& i) : id(meshId), vertices(v), indices(i), normals(v) { }
    Mesh(const Mesh& m) : id(m.id), vertices(m.vertices), indices(m.indices), normals(m.normals) { }
    virtual ~Mesh();
    std::vector<float> vertices;
    std::vector<float> normals;
    std::vector<unsigned int> indices;
    std::shared_ptr<Material> material;
    std::string id;
    bool cullFaces = true;
    virtual void GenerateVAO();
    virtual void Render() const;
    virtual void Bind() const;
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
#pragma once

#include <opengl.h>
#include <memory>
#include <string>
#include <vector>

#include "material.h"
#include "shader.h"
#include "camera.h"

class Mesh {
public:
    GLuint vao = NULL;
    GLuint vbo = NULL;
    GLuint ebo = NULL;
    GLuint normalBuffer = NULL;
    GLuint texCoordBuffer = NULL;
    glm::mat4 transformMatrix = glm::mat4(1.0f);
    ViewFrustum::AABB aabb;
    Mesh() = default;
    Mesh(const std::vector<float>& v, const std::vector<unsigned int>& i, const std::vector<float>& t, const std::vector<float>& n) : vertices(v), indices(i), texCoords(t), normals(n) { }
    Mesh(const std::vector<float>& v, const std::vector<unsigned int>& i, const std::vector<float>& t) : vertices(v), texCoords(t), indices(i), normals(v) { }
    Mesh(const std::vector<float>& v, const std::vector<unsigned int>& i) : vertices(v), indices(i), normals(v) {
        for (int i = 0; i < vertices.size() / 3; i++) {
            texCoords.push_back(0.0f);
            texCoords.push_back(0.0f);
        }
    }
    Mesh(const std::string& meshId, const std::vector<float>& v, const std::vector<unsigned int>& i, const std::vector<float>& t) : Mesh(v, i, t) { id = meshId; }
    Mesh(const std::string& meshId, const std::vector<float>& v, const std::vector<unsigned int>& i) : Mesh(v, i) { id = meshId; }
    Mesh(const Mesh& m) : id(m.id), vertices(m.vertices), indices(m.indices), texCoords(m.texCoords), normals(m.normals) { }
    virtual ~Mesh();
    std::vector<float> vertices;
    std::vector<float> normals;
    std::vector<float> texCoords;
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
            -0.5f, -0.5f,  0.5f,    0.5f, -0.5f,  0.5f,    0.5f,  0.5f,  0.5f,   -0.5f,  0.5f,  0.5f,
            -0.5f,  0.5f, -0.5f,    0.5f,  0.5f, -0.5f,    0.5f, -0.5f, -0.5f,   -0.5f, -0.5f, -0.5f,
             0.5f, -0.5f,  0.5f,    0.5f, -0.5f, -0.5f,    0.5f,  0.5f, -0.5f,    0.5f,  0.5f,  0.5f,
            -0.5f, -0.5f, -0.5f,   -0.5f, -0.5f,  0.5f,   -0.5f,  0.5f,  0.5f,   -0.5f,  0.5f, -0.5f,
             0.5f, -0.5f, -0.5f,   -0.5f, -0.5f, -0.5f,   -0.5f,  0.5f, -0.5f,    0.5f,  0.5f, -0.5f
        },
        {
            0,  1,  2,  2,  3,  0,
            19, 16, 17, 17, 18, 19,
            3,  2,  5,  5,  4,  3,
            7,  6,  1,  1,  0,  7,
            8,  9,  10, 10, 11, 8,
            12, 13, 14, 14, 15, 12
        },
        {
            0.0f, 1.0f,  1.0f, 1.0f,  1.0f, 0.0f,  0.0f, 0.0f,
            0.0f, 1.0f,  1.0f, 1.0f,  1.0f, 0.0f,  0.0f, 0.0f,
            0.0f, 1.0f,  1.0f, 1.0f,  1.0f, 0.0f,  0.0f, 0.0f,
            0.0f, 1.0f,  1.0f, 1.0f,  1.0f, 0.0f,  0.0f, 0.0f,
            0.0f, 1.0f,  1.0f, 1.0f,  1.0f, 0.0f,  0.0f, 0.0f
        }
    );

    static Mesh CUBE_WITHOUT_TEXCOORDS(
        "cube_notex",
        {
            -.5f,    .5f,    .5f,
            -.5f,   -.5f,    .5f,
             .5f,    .5f,    .5f,
             .5f,   -.5f,    .5f,
            -.5f,    .5f,   -.5f,
            -.5f,   -.5f,   -.5f,
             .5f,    .5f,   -.5f,
             .5f,   -.5f,   -.5f
        },
        {
            3, 2, 0, 1, 3, 0,
            7, 6, 2, 3, 7, 2,
            5, 4, 6, 7, 5, 6,
            1, 0, 4, 5, 1, 4,
            6, 4, 0, 2, 6, 0,
            1, 5, 7, 1, 7, 3,
        }
    );
};
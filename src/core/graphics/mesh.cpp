#include "mesh.h"

#include <iostream>
#include <spdlog/spdlog.h>

#include "texture.h"

void Mesh::GenerateVAO() {
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertices[0]), &vertices[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(indices[0]), &indices[0], GL_STATIC_DRAW);

    glGenBuffers(2, &texCoordBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, texCoordBuffer);
    glBufferData(GL_ARRAY_BUFFER, texCoords.size() * sizeof(texCoords[0]), &texCoords[0], GL_STATIC_DRAW);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(1);

    glGenBuffers(1, &normalBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(normals[0]), &normals[0], GL_STATIC_DRAW);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(2);
    
    glBindVertexArray(0);
}

void Mesh::Bind() const {
    glBindTexture(GL_TEXTURE_2D, material->GetTexture());
    glBindVertexArray(vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
}

void Mesh::Render() const {
    if (cullFaces)
        glEnable(GL_CULL_FACE);
    else
        glDisable(GL_CULL_FACE);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);
}

std::shared_ptr<Mesh> Meshes::CreateMeshInstance(const Mesh& m) {
    auto mesh = std::make_shared<Mesh>(m);
    mesh->GenerateVAO();
    return mesh;
}

Mesh::~Mesh() {
    if (vao != NULL)
        glDeleteVertexArrays(1, &vao);
    if (vbo != NULL)
        glDeleteBuffers(1, &vbo);
    if (ebo != NULL)
        glDeleteBuffers(1, &ebo);
    if (texCoordBuffer != NULL)
        glDeleteBuffers(1, &texCoordBuffer);
    if (normalBuffer != NULL)
        glDeleteBuffers(1, &normalBuffer);
}
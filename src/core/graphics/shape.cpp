#include "shape.h"

void Shape::GenerateVAO() {
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * numVertices * numVertexAttributes, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, numVertexAttributes, GL_FLOAT, GL_FALSE, numVertexAttributes * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Shape::Bind() {
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
}

Shape::~Shape() {
    if (vao != NULL)
        glDeleteVertexArrays(1, &vao);
    if (vbo != NULL)
        glDeleteBuffers(1, &vbo);
}
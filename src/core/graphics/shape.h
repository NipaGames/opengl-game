#pragma once
#include <opengl.h>

class Shape {
public:
    int numVertices = 6;
    int numVertexAttributes = 4;
    GLuint vao = NULL;
    GLuint vbo = NULL;
    ~Shape();
    void GenerateVAO();
    void Bind();
};
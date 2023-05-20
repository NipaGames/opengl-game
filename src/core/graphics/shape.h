#pragma once
#include <opengl.h>

class Shape {
private:
    float* vertexData = nullptr;
public:
    int numVertices = 6;
    int numVertexAttributes = 2;
    int stride = 2;
    GLuint vao = NULL;
    GLuint vbo = NULL;
    ~Shape();
    Shape() { }
    Shape(const Shape&);
    Shape(Shape&&);
    void GenerateVAO();
    void Bind();
    void SetVertexData(const float*);
};
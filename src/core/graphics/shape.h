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
    Shape() = default;
    Shape(const Shape&);
    Shape(Shape&&);
    Shape& operator=(const Shape&) = default;
    Shape& operator=(Shape&&) = default;
    void GenerateVAO();
    void Bind();
    void SetVertexData(float*, bool);
    void SetVertexData(const float*);
};
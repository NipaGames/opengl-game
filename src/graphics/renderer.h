#pragma once

#include <opengl.h>
#include <vector>

#include "graphics/camera.h"
#include "graphics/material.h"
#include "graphics/mesh.h"
#include "graphics/shader.h"
#include "graphics/shaders.h"

class Renderer {
private:
    GLFWwindow* window_;
    Camera camera_;
    Material exampleMaterial_ = Material(SHADER_EXAMPLE);
public:
    GLuint colorBuffer_;
    Renderer() { }
    virtual ~Renderer() { }
    void SetWindow(GLFWwindow* window) { window_ = window; }
    Renderer(GLFWwindow*);
    Camera& GetCamera() { return camera_; }
    bool Init();
    void GenerateModelVAOS();
    void Render();
    void UpdateCameraProjection(int, int);
};
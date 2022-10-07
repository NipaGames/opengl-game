#pragma once

#include <opengl.h>
#include <vector>

#include "graphics/camera.h"
#include "graphics/material.h"
#include "graphics/mesh.h"
#include "graphics/shader.h"
#include "graphics/shaders.h"

#include "entity/component/meshrenderer.h"

class Renderer {
private:
    GLFWwindow* window_;
    std::vector<MeshRenderer*> meshes_;
    Camera camera_;
public:
    Renderer() { }
    virtual ~Renderer() { }
    void SetWindow(GLFWwindow* window) { window_ = window; }
    Renderer(GLFWwindow*);
    Camera& GetCamera() { return camera_; }
    bool Init();
    void Render();
    void UpdateCameraProjection(int, int);
    void AddMeshRenderer(MeshRenderer* mesh) { meshes_.push_back(mesh); }
};
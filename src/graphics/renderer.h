#pragma once

#include <opengl.h>
#include <vector>
#include <unordered_map>

#include "graphics/camera.h"
#include "graphics/light.h"
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
    std::unordered_map<int, GLuint> shaders;
    std::vector<PointLight> pointLights;
    std::vector<DirectionalLight> directionalLights;
    std::vector<Spotlight> spotlights;
    glm::vec3 skyboxColor = glm::vec3(0.0);
    Renderer() { }
    virtual ~Renderer();
    void SetWindow(GLFWwindow* window) { window_ = window; }
    Renderer(GLFWwindow*);
    Camera& GetCamera() { return camera_; }
    bool Init();
    void Render();
    void UpdateCameraProjection(int, int);
    void AddMeshRenderer(MeshRenderer* mesh) { meshes_.push_back(mesh); }
};
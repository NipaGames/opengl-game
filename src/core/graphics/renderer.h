#pragma once

#include <opengl.h>
#include <vector>
#include <unordered_map>

#include "core/graphics/camera.h"
#include "core/graphics/light.h"
#include "core/graphics/material.h"
#include "core/graphics/mesh.h"
#include "core/graphics/shader.h"
#include "core/graphics/shaders.h"

#include "core/entity/component/meshrenderer.h"

class Renderer {
private:
    GLFWwindow* window_;
    GLuint fbo_;
    GLuint rbo_;
    GLuint textureColorBuffer_;
    GLuint MSAAFbo_;
    GLuint MSAATextureColorBuffer_;
    GLuint quadVao_, quadVbo_;
    std::vector<MeshRenderer*> meshes_;
    Camera camera_;
    Shader framebufferShader_;
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
    void Start();
    void Render();
    void UpdateCameraProjection(int, int);
    void AddMeshRenderer(MeshRenderer* mesh) { meshes_.push_back(mesh); }
};
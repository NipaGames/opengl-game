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
#include "core/ui/canvas.h"

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
    std::vector<Light::Light*> lights_;
    std::unordered_map<std::string, UI::Canvas> canvases_;
    Camera camera_;
    Shader framebufferShader_;
    Shader normalShader_;
    glm::ivec2 viewportSize_;
    int maxRenderedPointLights_ = 0;
    int maxRenderedDirLights_ = 0;
    int maxRenderedSpotlights_ = 0;
public:
    std::unordered_map<int, GLuint> shaders;
    glm::vec3 skyboxColor = glm::vec3(0.0);
    bool highlightNormals = false;
    Renderer() { }
    virtual ~Renderer();
    void UpdateLighting();
    void SetWindow(GLFWwindow* window) { window_ = window; }
    Renderer(GLFWwindow*);
    Camera& GetCamera() { return camera_; }
    bool Init();
    void Start();
    void Render();
    void UpdateCameraProjection(int, int);
    void AddMeshRenderer(MeshRenderer* mesh) { meshes_.push_back(mesh); }
    void AddLight(Light::Light*);
    void RemoveLight(Light::Light*);
    UI::Canvas& CreateCanvas(std::string);
};
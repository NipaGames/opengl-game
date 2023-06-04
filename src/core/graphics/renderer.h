#pragma once

#include <opengl.h>
#include <vector>
#include <unordered_map>

#include "camera.h"
#include "material.h"
#include "mesh.h"
#include "shader.h"
#include "shaders.h"
#include "shape.h"
#include "component/light.h"
#include "component/meshrenderer.h"
#include <core/ui/canvas.h>


class Renderer {
private:
    GLFWwindow* window_;
    GLuint fbo_;
    GLuint rbo_;
    GLuint textureColorBuffer_;
    GLuint MSAAFbo_;
    GLuint MSAATextureColorBuffer_;
    Shape framebufferShape_;
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
    std::vector<GLuint> shaders_;
    int entitiesOnFrustum_ = 0;
public:
    std::unordered_map<std::string, std::shared_ptr<Material>> materials;
    glm::vec3 skyboxColor = glm::vec3(0.0);
    bool highlightNormals = false;
    bool showHitboxes = false;
    Renderer() = default;
    virtual ~Renderer();
    void UpdateLighting();
    void SetWindow(GLFWwindow* window) { window_ = window; }
    Renderer(GLFWwindow*);
    Camera& GetCamera() { return camera_; }
    bool Init();
    void Start();
    void Render();
    void UpdateCameraProjection(int, int);
    void AddMeshRenderer(MeshRenderer*);
    void RemoveMeshRenderer(MeshRenderer*);
    void AddLight(Light::Light*);
    void RemoveLight(Light::Light*);
    void CopyShadersFromResources();
    UI::Canvas& CreateCanvas(std::string);
    UI::Canvas& GetCanvas(const std::string&);
    int CountEntitiesOnFrustum() { return entitiesOnFrustum_; }
};
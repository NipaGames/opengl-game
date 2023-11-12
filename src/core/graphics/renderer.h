#pragma once

#include <opengl.h>
#include <vector>
#include <unordered_map>

#include "camera.h"
#include "shader.h"
#include "mesh.h"
#include "shape.h"

// forward declarations
class MeshRenderer;
class PostProcessing;
namespace UI {
    class Canvas;
};
namespace Lights {
    class Light;
};
namespace Resources {
    struct VideoSettings;
};

class Renderer {
private:
    GLFWwindow* window_;
    GLuint fbo_;
    GLuint rbo_;
    GLuint framebufferTexture_;
    GLuint MSAAFbo_;
    GLuint MSAATextureColorBuffer_;
    Shape framebufferShape_;
    std::vector<MeshRenderer*> meshes_;
    std::vector<Lights::Light*> lights_;
    std::unordered_map<std::string, UI::Canvas> canvases_;
    Camera camera_ = Camera();
    Shader framebufferShader_;
    Shader normalShader_;
    glm::ivec2 viewportSize_;
    int maxRenderedPointLights_ = 0;
    int maxRenderedDirLights_ = 0;
    int maxRenderedSpotlights_ = 0;
    std::vector<GLuint> shaders_;
    std::vector<MeshRenderer*> entitiesOnFrustum_;
    std::unordered_map<std::string, std::shared_ptr<Material>> materials_;
public:
    std::shared_ptr<Mesh> skybox = nullptr;
    Texture::TextureID skyboxTexture = NULL;
    glm::vec3 skyboxColor = glm::vec3(0.0);
    bool highlightNormals = false;
    bool showHitboxes = false;
    bool showAabbs = false;
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
    void AddLight(Lights::Light*);
    void RemoveLight(Lights::Light*);
    void CopyShadersFromResources();
    void UpdateFrustum();
    void SortMeshesByDistance();
    void UpdateVideoSettings(const Resources::VideoSettings&);
    void ApplyPostProcessing(const PostProcessing&);
    UI::Canvas& CreateCanvas(std::string);
    UI::Canvas& GetCanvas(const std::string&);
    size_t CountEntitiesOnFrustum() { return entitiesOnFrustum_.size(); }
    std::shared_ptr<Material> GetMaterial(const std::string&);
    std::unordered_map<std::string, std::shared_ptr<Material>>& GetMaterials() { return materials_; }
    const std::vector<GLuint>& GetShaders() { return shaders_; }
};
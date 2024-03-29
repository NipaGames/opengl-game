#include "renderer.h"

#include "postprocessing.h"
#include "component/light.h"
#include "component/meshrenderer.h"

#include <core/game.h>
#include <core/gamewindow.h>
#include <core/physics/physics.h>
#include <core/ui/canvas.h>

#include <spdlog/spdlog.h>

Renderer::Renderer(GLFWwindow* window) {
    SetWindow(window);
}

Renderer::~Renderer() {
    for (GLuint s : shaders_)
        glDeleteProgram(s);

    glBindRenderbuffer(GL_RENDERBUFFER, fbo_);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
        glDeleteFramebuffers(1, &fbo_);
    glBindRenderbuffer(GL_RENDERBUFFER, MSAAFbo_);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
        glDeleteFramebuffers(1, &MSAAFbo_);

    shaders_.clear();

    for (const auto& [k, v] : canvases_) {
        if (v->isOwnedByRenderer)
            delete v;
    }
    canvases_.clear();
}

bool Renderer::Init() {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glEnable(GL_DEPTH_TEST);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  

    glfwWindowHint(GLFW_SAMPLES, 4);
    glEnable(GL_MULTISAMPLE);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // debug line rendering

    float aspectRatio = (float) 16 / (float) 9;

    camera_.aspectRatio = aspectRatio;
    camera_.projectionMatrix = glm::perspective(glm::radians(camera_.fov), aspectRatio, camera_.clippingNear, camera_.clippingFar);
    camera_.pos = glm::vec3(0.0f, 0.0f, -10.0f);
    
    glGenFramebuffers(1, &MSAAFbo_);
    glBindFramebuffer(GL_FRAMEBUFFER, MSAAFbo_);

    glGenTextures(1, &MSAATextureColorBuffer_);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, MSAATextureColorBuffer_);
    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGB, BASE_WIDTH, BASE_HEIGHT, GL_TRUE);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, MSAATextureColorBuffer_, 0);

    glGenRenderbuffers(1, &rbo_);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo_);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, BASE_WIDTH, BASE_HEIGHT);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo_);

    glGenFramebuffers(1, &fbo_);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo_);

    glGenTextures(1, &framebufferTexture_);
    glBindTexture(GL_TEXTURE_2D, framebufferTexture_);
    GAME->resources.textureManager.Set("FRAMEBUFFER", framebufferTexture_);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, BASE_WIDTH, BASE_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, framebufferTexture_, 0);


    framebufferShader_ = Shader(Shaders::ShaderID::FRAMEBUFFER);
    normalShader_ = Shader(Shaders::ShaderID::HIGHLIGHT_NORMALS);

    framebufferShape_.numVertexAttributes = 4;
    framebufferShape_.GenerateVAO();
    framebufferShape_.Bind();
    const float quadVertices[] = {
        // positions   // texCoords
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };
    framebufferShape_.SetVertexData(quadVertices);

    glfwGetWindowSize(window_, &viewportSize_.x, &viewportSize_.y);

    std::shared_ptr<Material> missingMaterial = std::make_shared<Material>(Shaders::ShaderID::STROBE_UNLIT);
    missingMaterial->SetShaderUniform("colors[0]", glm::vec3(0.0f));
    missingMaterial->SetShaderUniform("colors[1]", glm::vec3(1.0f, 0.0f, 1.0f));
    missingMaterial->SetShaderUniform("strobeInterval", .25f);
    missingMaterial->SetShaderUniform("colorCount", 2);
    materials_[MATERIAL_MISSING] = missingMaterial;

    return true;
}

void Renderer::UpdateLighting() {
    for (GLuint shader : shaders_) {
        glUseProgram(shader);
        Lights::ResetIndices();
        for (auto l : lights_) {
            while (Lights::IsReserved(Lights::LIGHTS_INDEX)) {
                Lights::LIGHTS_INDEX++;
            }
            l->UseAsNext();
            l->ApplyLight(shader);
        }
        if (Lights::LIGHTS_INDEX < maxRenderedLights_) {
            for (int i = Lights::LIGHTS_INDEX; i < maxRenderedLights_; i++) {
                if (!Lights::IsReserved(i))
                    glUniform1i(glGetUniformLocation(shader, std::string("lights[" + std::to_string(i) + "].enabled").c_str()), GL_FALSE);
            }
        }
    }
    maxRenderedLights_ = std::max(Lights::LIGHTS_INDEX, maxRenderedLights_);
}

void Renderer::CopyShadersFromResources() {
    const auto& shaderMap = GAME->resources.shaderManager.GetAll();
    shaders_.clear();
    std::transform(shaderMap.begin(), shaderMap.end(), std::back_inserter(shaders_), [](const auto& s) { return s.second; });
}

void Renderer::Start() {
    CopyShadersFromResources();
    UpdateLighting();
    UpdateFrustum();
}

void Renderer::SortMeshesByDistance() {
    std::sort(entitiesOnFrustum_.begin(), entitiesOnFrustum_.end(), [&] (const auto& mesh0, const auto& mesh1) {
        return glm::length(camera_.pos - mesh0->parent->transform->position) > glm::length(camera_.pos - mesh1->parent->transform->position);
    });
}

void Renderer::UpdateFrustum() {
    entitiesOnFrustum_.clear();
    std::copy_if(meshes_.begin(), meshes_.end(), std::back_inserter(entitiesOnFrustum_), [&] (auto mesh) {
        return mesh->IsOnFrustum(camera_.frustum);
    });
}

void Renderer::Render() {
    // first pass (draw into framebuffer)
    glBindFramebuffer(GL_FRAMEBUFFER, MSAAFbo_);
    
    glClearColor(skyboxColor.r, skyboxColor.g, skyboxColor.b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 viewMatrix = glm::lookAt(camera_.pos, camera_.pos + camera_.front, camera_.up);

    glUseProgram(0);
    for (auto mesh : meshes_) {
        if (!mesh->isStatic)
            mesh->CalculateModelMatrix();
    }
    std::vector<const MeshRenderer*> lateMeshes;
    std::vector<const MeshRenderer*> afterPostProcessingMeshes;
    for (const MeshRenderer* mesh : entitiesOnFrustum_) {
        if (mesh->renderLate) {
            lateMeshes.push_back(mesh);
            continue;
        }
        else if (mesh->renderAfterPostProcessing) {
            afterPostProcessingMeshes.push_back(mesh);
            continue;
        }
        mesh->Render(camera_.projectionMatrix, viewMatrix, nullptr, showAabbs);
    }

    // draw skybox
    if (skybox != nullptr) {
        glDepthFunc(GL_LEQUAL);
        glCullFace(GL_FRONT);
        glEnable(GL_DEPTH_CLAMP);

        const Shader* shader = &skybox->material->GetShader();
        shader->Use();
        glm::mat4 skyboxView = glm::mat4(glm::mat3(viewMatrix));
        shader->SetUniform("view", skyboxView);
        shader->SetUniform("projection", camera_.projectionMatrix);
        shader->SetUniform("clippingFar", camera_.clippingFar);
        
        glBindVertexArray(skybox->vao);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skybox->ebo);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);

        skybox->Render();
        glBindVertexArray(0);
        glDepthFunc(GL_LESS);
        glCullFace(GL_BACK);
        glDisable(GL_DEPTH_CLAMP);
    }

    for (const MeshRenderer* mesh : lateMeshes) {
        mesh->Render(camera_.projectionMatrix, viewMatrix, nullptr, showAabbs);
    }

    if (highlightNormals) {
        for (auto meshRenderer : meshes_) {
            meshRenderer->Render(camera_.projectionMatrix, viewMatrix, &normalShader_);
        }
    }
    if (showHitboxes)
        Physics::dynamicsWorld->debugDrawWorld();

    // second pass (draw framebuffer onto screen)
    glBindFramebuffer(GL_READ_FRAMEBUFFER, MSAAFbo_);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo_);
    glBlitFramebuffer(0, 0, viewportSize_.x, viewportSize_.y, 0, 0, viewportSize_.x, viewportSize_.y, GL_COLOR_BUFFER_BIT, GL_NEAREST);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);

    framebufferShader_.Use();
    framebufferShape_.Bind();
    glBindTexture(GL_TEXTURE_2D, framebufferTexture_);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glEnable(GL_DEPTH_TEST);
    glClear(GL_DEPTH_BUFFER_BIT);
    for (const MeshRenderer* mesh : afterPostProcessingMeshes) {
        mesh->Render(camera_.projectionMatrix, viewMatrix, nullptr, showAabbs);
    }
    
    glDisable(GL_DEPTH_TEST);
    for (auto& c : canvases_) {
        c.second->Draw();
    }

    glEnable(GL_DEPTH_TEST);
    glfwSwapBuffers(window_);
}

void Renderer::UpdateCameraProjection(int width, int height) {
    viewportSize_ = glm::ivec2(width, height);
    glViewport(0, 0, width, height);

    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, MSAATextureColorBuffer_);
    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGB, width, height, GL_TRUE);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo_);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, width, height);
    glBindTexture(GL_TEXTURE_2D, framebufferTexture_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

    camera_.aspectRatio = (float) width / (float) height;
    camera_.projectionMatrix = glm::perspective(glm::radians(camera_.fov), camera_.aspectRatio, camera_.clippingNear, camera_.clippingFar);
    
    for (auto& c : canvases_) {
        c.second->UpdateWindowSize();
    }
}

void Renderer::ApplyPostProcessing(const PostProcessing& postProcessing) {
    framebufferShader_.Use();
    postProcessing.ApplyUniforms(framebufferShader_);
    glUseProgram(0);
}

void Renderer::UpdateVideoSettings(const Resources::VideoSettings& settings) {
    framebufferShader_.Use();
    framebufferShader_.SetUniform("cfg.gamma", settings.gamma);
    framebufferShader_.SetUniform("cfg.contrast", settings.contrast);
    framebufferShader_.SetUniform("cfg.brightness", settings.brightness);
    framebufferShader_.SetUniform("cfg.saturation", settings.saturation);
    glUseProgram(0);

    camera_.fov = settings.fov;

    int w, h;
    glfwGetWindowSize(window_, &w, &h);
    UpdateCameraProjection(w, h);
}

void Renderer::AddLight(Lights::Light* light) {
    lights_.push_back(light);
    light->isAdded = true;
}

void Renderer::RemoveLight(Lights::Light* light) {
    if (lights_.size() == 0) return;
    lights_.erase(std::remove(lights_.begin(), lights_.end(), light), lights_.end());
}

void Renderer::AddMeshRenderer(MeshRenderer* mesh) {
    meshes_.push_back(mesh);
    mesh->isAdded = true;
}

void Renderer::RemoveMeshRenderer(MeshRenderer* mesh) {
    if (meshes_.size() == 0) return;
    meshes_.erase(std::remove(meshes_.begin(), meshes_.end(), mesh), meshes_.end());
}

void Renderer::CleanUpEntities() {
    for (const auto& [k, v] : canvases_) {
        if (v->isOwnedByRenderer)
            delete v;
    }
    canvases_.clear();
    meshes_.clear();
    entitiesOnFrustum_.clear();
    lights_.clear();
}

std::shared_ptr<Material> Renderer::GetMaterial(const std::string& mat) {
    if (materials_.count(mat) == 0)
        return materials_.at(MATERIAL_MISSING);
    return materials_.at(mat);
}

UI::Canvas& Renderer::CreateCanvas(std::string id) {
    UI::Canvas* c = new UI::Canvas();
    c->isOwnedByRenderer = true;
    auto [ it, inserted ] = canvases_.insert({ id, c });
    it->second->GenerateBackgroundShape();
    return *it->second;
}

UI::Canvas& Renderer::GetCanvas(const std::string& id) {
    return *canvases_.at(id);
}

void Renderer::AssignCanvas(const std::string& id, UI::Canvas* c) {
    canvases_.insert({ id, c });
}

void Renderer::MoveCanvas(const std::string& id, UI::Canvas* c) {
    AssignCanvas(id, c);
    c->isOwnedByRenderer = true;
}
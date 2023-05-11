#include "core/graphics/renderer.h"

#include "core/gamewindow.h"
#include "core/graphics/mesh.h"

#include <spdlog/spdlog.h>

Renderer::Renderer(GLFWwindow* window) {
    SetWindow(window);
}

Renderer::~Renderer() {
    for (auto[k, v] : shaders)
        glDeleteProgram(v);

    glBindRenderbuffer(GL_RENDERBUFFER, fbo_);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
        glDeleteFramebuffers(1, &fbo_);
    glBindRenderbuffer(GL_RENDERBUFFER, MSAAFbo_);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
        glDeleteFramebuffers(1, &MSAAFbo_);

    glDeleteBuffers(1, &quadVao_);
    glDeleteBuffers(1, &quadVbo_);

    shaders.clear();
}

bool Renderer::Init() {
    glClearColor(skyboxColor.x, skyboxColor.y, skyboxColor.z, 1.0f);
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

    camera_.projectionMatrix = glm::perspective(glm::radians(camera_.fov), aspectRatio, 0.1f, 500.0f);
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

    glGenTextures(1, &textureColorBuffer_);
    glBindTexture(GL_TEXTURE_2D, textureColorBuffer_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, BASE_WIDTH, BASE_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorBuffer_, 0);


    framebufferShader_ = Shader(SHADER_FRAMEBUFFER);
    normalShader_ = Shader(SHADER_HIGHLIGHT_NORMALS);

    // create a quad for the framebuffer
    // yeah this is really janky but i can't be bothered to write a shape interface for 2d objects right now

    // also, this is straight from https://learnopengl.com/code_viewer_gh.php?code=src/4.advanced_opengl/5.1.framebuffers/framebuffers.cpp
    float quadVertices[] = {
        // positions   // texCoords
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };

    glGenVertexArrays(1, &quadVao_);
    glGenBuffers(1, &quadVbo_);
    glBindVertexArray(quadVao_);
    glBindBuffer(GL_ARRAY_BUFFER, quadVbo_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    glfwGetWindowSize(window_, &viewportSize_.x, &viewportSize_.y);

    return true;
}

void Renderer::UpdateLighting() {
    for (const auto&[_, shader] : shaders) {
        glUseProgram(shader);
        Light::ResetIndices();
        for (auto l : lights_) {
            l->ApplyLight(shader);
        }
        if (Light::POINT_LIGHTS_INDEX < maxRenderedPointLights_) {
            for (int i = Light::POINT_LIGHTS_INDEX; i < maxRenderedPointLights_; i++) {
                glUniform1i(glGetUniformLocation(shader, std::string("pointLights[" + std::to_string(i) + "].enabled").c_str()), GL_FALSE);
            }
        }
        if (Light::DIRECTIONAL_LIGHTS_INDEX < maxRenderedDirLights_) {
            for (int i = Light::DIRECTIONAL_LIGHTS_INDEX; i < maxRenderedDirLights_; i++) {
                glUniform1i(glGetUniformLocation(shader, std::string("directionalLights[" + std::to_string(i) + "].enabled").c_str()), GL_FALSE);
            }
        }
        if (Light::SPOTLIGHTS_INDEX < maxRenderedSpotlights_) {
            for (int i = Light::SPOTLIGHTS_INDEX; i < maxRenderedSpotlights_; i++) {
                glUniform1i(glGetUniformLocation(shader, std::string("spotlights[" + std::to_string(i) + "].enabled").c_str()), GL_FALSE);
            }
        }
    }
    maxRenderedPointLights_ = std::max(Light::POINT_LIGHTS_INDEX, maxRenderedPointLights_);
    maxRenderedDirLights_ = std::max(Light::DIRECTIONAL_LIGHTS_INDEX, maxRenderedDirLights_);
    maxRenderedSpotlights_ = std::max(Light::SPOTLIGHTS_INDEX, maxRenderedSpotlights_);
}

void Renderer::Start() {
    UpdateLighting();
}

void Renderer::Render() {
    // first pass (draw into framebuffer)
    glBindFramebuffer(GL_FRAMEBUFFER, MSAAFbo_);
    glClearColor(skyboxColor.x, skyboxColor.y, skyboxColor.z, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    glm::mat4 viewMatrix = glm::lookAt(camera_.pos, camera_.pos + camera_.front, camera_.up);
    glUseProgram(0);

    for (auto meshRenderer : meshes_) {
        meshRenderer->Render(camera_.projectionMatrix, viewMatrix);
    }
    if (highlightNormals) {
        for (auto meshRenderer : meshes_) {
            meshRenderer->Render(camera_.projectionMatrix, viewMatrix, &normalShader_);
        }
    }

    // second pass (draw framebuffer onto screen)
    glBindFramebuffer(GL_READ_FRAMEBUFFER, MSAAFbo_);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo_);
    glBlitFramebuffer(0, 0, viewportSize_.x, viewportSize_.y, 0, 0, viewportSize_.x, viewportSize_.y, GL_COLOR_BUFFER_BIT, GL_NEAREST);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDisable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT);

    framebufferShader_.Use();
    glBindVertexArray(quadVao_);
    glBindTexture(GL_TEXTURE_2D, textureColorBuffer_);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    for (auto c : canvases_) {
        c.second.Draw();
    }

    glfwSwapBuffers(window_);
}

void Renderer::UpdateCameraProjection(int width, int height) {
    viewportSize_ = glm::ivec2(width, height);
    glViewport(0, 0, width, height);

    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, MSAATextureColorBuffer_);
    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGB, width, height, GL_TRUE);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo_);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, width, height);
    glBindTexture(GL_TEXTURE_2D, textureColorBuffer_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

    camera_.projectionMatrix = glm::perspective(glm::radians(camera_.fov), (float) width / (float) height, 0.1f, 500.0f);
    
    for (auto c : canvases_) {
        c.second.UpdateWindowSize();
    }
}

void Renderer::AddLight(Light::Light* light) {
    lights_.push_back(light);
    light->isAdded = true;
}

void Renderer::RemoveLight(Light::Light* light) {
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

UI::Canvas& Renderer::CreateCanvas(std::string id) {
    canvases_.insert({ id, UI::Canvas() });
    return canvases_.at(id);
}

UI::Canvas& Renderer::GetCanvas(const std::string& id) {
    return canvases_.at(id);
}
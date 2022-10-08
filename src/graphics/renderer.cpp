#include "graphics/renderer.h"

#include "gamewindow.h"
#include "graphics/mesh.h"

#include <spdlog/spdlog.h>

Renderer::Renderer(GLFWwindow* window) {
    SetWindow(window);
}

bool Renderer::Init() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glShadeModel(GL_SMOOTH);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    float aspectRatio = (float) 16 / (float) 9;

    camera_.projectionMatrix = glm::perspective(glm::radians(camera_.fov), aspectRatio, 0.1f, 500.0f);
    camera_.pos = glm::vec3(0.0f, 0.0f, -10.0f);

    return true;
}

void Renderer::Render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.1f, 0.0f, 0.1f, 1.0f);

    glm::mat4 viewMatrix = glm::lookAt(camera_.pos, camera_.pos + camera_.front, camera_.up);

    for (auto meshRenderer : meshes_) {
        meshRenderer->Render(camera_.projectionMatrix * viewMatrix);
    }

    glBindVertexArray(0);
    glfwSwapBuffers(window_);
}

void Renderer::UpdateCameraProjection(int width, int height) {
    glViewport(0, 0, width, height);
    camera_.projectionMatrix = glm::perspective(glm::radians(camera_.fov), (float) width / (float) height, 0.1f, 500.0f);
}
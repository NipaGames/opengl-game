#include "graphics/renderer.h"

#include "gamewindow.h"
#include "graphics/mesh.h"

#include <spdlog/spdlog.h>

Renderer::Renderer(GLFWwindow* window) {
    SetWindow(window);
}

bool Renderer::Init() {
    glClearColor(skyboxColor.x, skyboxColor.y, skyboxColor.z, 1.0f);
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
    glClearColor(skyboxColor.x, skyboxColor.y, skyboxColor.z, 1.0f);

    glm::mat4 viewMatrix = glm::lookAt(camera_.pos, camera_.pos + camera_.front, camera_.up);

    for (auto shader : shaders) {
        glUseProgram(shader.second);
        for (int i = 0; i < pointLights.size(); i++) {
            PointLight& light = pointLights[i];
            std::string lightUniform = "pointLights[" + std::to_string(i) + "]";
            glUniform3f(glGetUniformLocation(shader.second, std::string(lightUniform + ".pos").c_str()), light.pos.x, light.pos.y, light.pos.z);
            glUniform3f(glGetUniformLocation(shader.second, std::string(lightUniform + ".color").c_str()), light.color.x, light.color.y, light.color.z);
            glUniform1f(glGetUniformLocation(shader.second, std::string(lightUniform + ".range").c_str()), light.range);
            glUniform1f(glGetUniformLocation(shader.second, std::string(lightUniform + ".intensity").c_str()), light.intensity);
        }
        for (int i = 0; i < directionalLights.size(); i++) {
            DirectionalLight& light = directionalLights[i];
            std::string lightUniform = "directionalLights[" + std::to_string(i) + "]";
            glUniform3f(glGetUniformLocation(shader.second, std::string(lightUniform + ".dir").c_str()), light.dir.x, light.dir.y, light.dir.z);
            glUniform3f(glGetUniformLocation(shader.second, std::string(lightUniform + ".color").c_str()), light.color.x, light.color.y, light.color.z);
            glUniform1f(glGetUniformLocation(shader.second, std::string(lightUniform + ".intensity").c_str()), light.intensity);
        }
        for (int i = 0; i < spotlights.size(); i++) {
            Spotlight& light = spotlights[i];
            std::string lightUniform = "spotlights[" + std::to_string(i) + "]";
            glUniform3f(glGetUniformLocation(shader.second, std::string(lightUniform + ".pos").c_str()), light.pos.x, light.pos.y, light.pos.z);
            glUniform3f(glGetUniformLocation(shader.second, std::string(lightUniform + ".dir").c_str()), light.dir.x, light.dir.y, light.dir.z);
            glUniform3f(glGetUniformLocation(shader.second, std::string(lightUniform + ".color").c_str()), light.color.x, light.color.y, light.color.z);
            glUniform1f(glGetUniformLocation(shader.second, std::string(lightUniform + ".cutOffMin").c_str()), light.cutOffMin);
            glUniform1f(glGetUniformLocation(shader.second, std::string(lightUniform + ".cutOffMax").c_str()), light.cutOffMax);
            glUniform1f(glGetUniformLocation(shader.second, std::string(lightUniform + ".range").c_str()), light.range);
            glUniform1f(glGetUniformLocation(shader.second, std::string(lightUniform + ".intensity").c_str()), light.intensity);
        }
    }
    glUseProgram(0);

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
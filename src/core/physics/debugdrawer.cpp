#include "debugdrawer.h"

#include <spdlog/spdlog.h>
#include <opengl.h>
#include <core/game.h>

using namespace Physics;

DebugDrawer::~DebugDrawer() {
    glDeleteVertexArrays(1, &vao_);
    glDeleteBuffers(1, &vbo_);
}

DebugDrawer::DebugDrawer() : shader_(Shaders::ShaderID::LINE) {
    glGenVertexArrays(1, &vao_);
    glGenBuffers(1, &vbo_);
}

void DebugDrawer::drawLine(const btVector3& from, const btVector3& to, const btVector3& color) {
    vertices_.insert(vertices_.end(), { from.getX(), from.getY(), from.getZ(), to.getX(), to.getY(), to.getZ() });
}

void DebugDrawer::clearLines() {
    vertices_.clear();
}

void DebugDrawer::flushLines() {
    shader_.Use();
    Camera& cam = GAME->GetRenderer().GetCamera();
    glm::mat4 viewMatrix = glm::lookAt(cam.pos, cam.pos + cam.front, cam.up);
    shader_.SetUniform("view", viewMatrix);
    shader_.SetUniform("projection", cam.projectionMatrix);

    glBindVertexArray(vao_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER, vertices_.size() * sizeof(float), &vertices_.front(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), reinterpret_cast<void*>(0));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));
    glDrawArrays(GL_POINTS, 0, (GLsizei) vertices_.size());
}

void DebugDrawer::reportErrorWarning(const char* msg) {
    spdlog::warn("(DebugDrawer) " + std::string(msg));
}
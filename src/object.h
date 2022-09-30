#pragma once

#include <opengl.h>

#include "graphics/mesh.h"

class Object {
private:
public:
    Object(const std::shared_ptr<Mesh>& m) : mesh(m) { }

    const std::shared_ptr<Mesh>& mesh;
    int shader;
    glm::vec3 position = glm::vec3(0.0f);
    glm::quat rotation = glm::quat(0.0f, 0.0f, 0.0f, 0.0f);

    void Update();
    void Render();
};
#pragma once

#include "graphics/camera.h"
#include "graphics/material.h"
#include "graphics/mesh.h"
#include "entity/component.h"

#include <opengl.h>
#include <memory.h>

class MeshRenderer : public Component {
private:
    GLuint colorBuffer_;
public:
    virtual ~MeshRenderer() { }

    std::vector<std::shared_ptr<Mesh>> meshes;

    virtual void Start() override;
    virtual void Render(const glm::mat4&);
};
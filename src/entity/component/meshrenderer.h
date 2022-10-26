#pragma once

#include "graphics/camera.h"
#include "graphics/material.h"
#include "graphics/mesh.h"
#include "entity/component.h"

#include <opengl.h>
#include <memory.h>

class MeshRenderer : public Component<MeshRenderer> {
public:
    virtual ~MeshRenderer() { }

    std::vector<std::shared_ptr<Mesh>> meshes;

    void Start();
    void Render(const glm::mat4&);
};
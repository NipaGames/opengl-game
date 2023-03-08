#pragma once

#include "core/graphics/camera.h"
#include "core/graphics/material.h"
#include "core/graphics/mesh.h"
#include "core/entity/component.h"

#include <opengl.h>
#include <memory.h>

class MeshRenderer : public Component<MeshRenderer> {
public:
    virtual ~MeshRenderer() { }

    std::vector<std::shared_ptr<Mesh>> meshes;

    void Start();
    void Render(const glm::mat4&, const glm::mat4&, const Shader* shader = nullptr);
};
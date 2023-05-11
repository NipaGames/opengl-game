#pragma once

#include "core/graphics/camera.h"
#include "core/graphics/material.h"
#include "core/graphics/mesh.h"
#include "core/entity/component.h"

#include <opengl.h>
#include <memory.h>

class MeshRenderer : public Component<MeshRenderer> {
private:
public:
    bool isAdded = false;
    DEFINE_COMPONENT_DATA_VALUE_VECTOR(std::shared_ptr<Mesh>, meshes);

    virtual ~MeshRenderer();

    virtual void Start();
    virtual void Render(const glm::mat4&, const glm::mat4&, const Shader* = nullptr);
};
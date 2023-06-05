#pragma once

#include "../camera.h"
#include "../material.h"
#include "../mesh.h"
#include <core/entity/component.h>

#include <opengl.h>
#include <memory.h>

class MeshRenderer : public Component<MeshRenderer> {
private:
    mutable glm::mat4 modelMatrix_;
    ViewFrustum::AABB aabb_;
public:
    bool isAdded = false;
    DEFINE_COMPONENT_DATA_VALUE_VECTOR(std::shared_ptr<Mesh>, meshes);

    virtual ~MeshRenderer();
    virtual void CalculateModelMatrix();

    virtual void Start();
    virtual bool IsOnFrustum(const ViewFrustum&) const;
    virtual void Render(const glm::mat4&, const glm::mat4&, const Shader* = nullptr) const;
};
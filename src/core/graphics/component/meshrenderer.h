#pragma once

#include "../camera.h"
#include "../material.h"
#include "../mesh.h"
#include "../object.h"
#include <core/entity/component.h>

#include <opengl.h>
#include <memory.h>

class MeshRenderer : public Component<MeshRenderer> {
private:
    mutable glm::mat4 modelMatrix_;
    ViewFrustum::AABB aabb_;
public:
    bool isAdded = false;
    // enable this if the object transform doesn't update, no need to calculate model matrices every frame that way
    DEFINE_COMPONENT_DATA_VALUE(bool, isStatic, false);
    DEFINE_COMPONENT_DATA_VALUE(bool, alwaysOnFrustum, false);
    DEFINE_COMPONENT_DATA_VALUE(std::string, object, "");
    DEFINE_COMPONENT_DATA_VALUE_VECTOR(std::shared_ptr<Mesh>, meshes);
    DEFINE_COMPONENT_DATA_VALUE(bool, copyMeshes, false);

    virtual ~MeshRenderer();
    virtual void CalculateModelMatrix();

    virtual void Start();
    virtual bool IsOnFrustum(const ViewFrustum&) const;
    virtual void Render(const glm::mat4&, const glm::mat4&, const Shader* = nullptr, bool = false) const;
    const ViewFrustum::AABB& GetAABB() const { return aabb_; }
};
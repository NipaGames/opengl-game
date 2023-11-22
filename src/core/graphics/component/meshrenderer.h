#pragma once

#include "../camera.h"
#include "../material.h"
#include "../mesh.h"
#include "../object.h"
#include <core/entity/component.h>

#include <opengl.h>
#include <memory>

class MeshRenderer : public Component<MeshRenderer> {
private:
    mutable glm::mat4 modelMatrix_;
    ViewFrustum::AABB aabb_;
public:
    bool isAdded = false;
    // enable this if the object transform doesn't update, no need to calculate model matrices every frame that way
    DEFINE_COMPONENT_DATA_VALUE(bool, isStatic, false);
    DEFINE_COMPONENT_DATA_VALUE(bool, alwaysOnFrustum, false);
    DEFINE_COMPONENT_DATA_VALUE(bool, disableDepthTest, false);
    DEFINE_COMPONENT_DATA_VALUE(std::string, object, "");
    DEFINE_COMPONENT_DATA_VALUE_VECTOR(std::shared_ptr<Mesh>, meshes);
    DEFINE_COMPONENT_DATA_VALUE(bool, copyMeshes, false);
    DEFINE_COMPONENT_DATA_VALUE_DEFAULT(Material, customMaterial);
    DEFINE_COMPONENT_DATA_VALUE(bool, useCustomMaterial, false);
    // render over skybox and any other entities
    DEFINE_COMPONENT_DATA_VALUE(bool, renderLate, false);
    DEFINE_COMPONENT_DATA_VALUE(bool, renderAfterPostProcessing, false);

    virtual ~MeshRenderer();
    virtual void CalculateModelMatrix();
    virtual const Shader& GetMaterialShader(const std::shared_ptr<Material>&) const;
    virtual void UpdateUniforms(const Shader&, const glm::mat4&, const glm::mat4&, const glm::mat4&) const;
    virtual void UseMaterial(const std::shared_ptr<Material>&) const;

    virtual void Start();
    virtual bool IsOnFrustum(const ViewFrustum&) const;
    virtual void Render(const glm::mat4&, const glm::mat4&, const Shader* = nullptr, bool = false) const;
    const ViewFrustum::AABB& GetAABB() const { return aabb_; }
};
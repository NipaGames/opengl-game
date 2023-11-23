#pragma once

#include "../physics.h"
#include <core/entity/component.h>

namespace Physics {
    enum class ColliderConstructor {
        TRANSFORM,
        AABB,
        MESH
    };

    // would be great to have a separate collider component
    class RigidBody : public Component<RigidBody> {
    private:
        bool enableDebugVisualization_ = true;
        bool enableRotation_ = true;
        glm::vec3 colliderOriginOffset_ = glm::vec3(0.0f);
        btTriangleIndexVertexArray* meshData_ = nullptr;
        btCollisionShape* CreateMeshCollider();
    public:
        btRigidBody* rigidBody = nullptr;
        DEFINE_COMPONENT_DATA_VALUE(btCollisionShape*, collider, nullptr);
        DEFINE_COMPONENT_DATA_VALUE(ColliderConstructor, colliderFrom, ColliderConstructor::TRANSFORM);
        DEFINE_COMPONENT_DATA_VALUE(float, mass, 1.0f);
        DEFINE_COMPONENT_DATA_VALUE(bool, interpolate, false);
        DEFINE_COMPONENT_DATA_VALUE(bool, overwriteTransform, true);
        DEFINE_COMPONENT_DATA_VALUE(bool, doesMassAffectGravity, false);
        DEFINE_COMPONENT_DATA_VALUE(bool, disableCollisions, false);
        ~RigidBody();
        void Start();
        void Update();
        void FixedUpdate();
        void SetPos(const glm::vec3&);
        void UpdateTransform();
        void CopyTransform();
        void EnableDebugVisualization(bool);
        void EnableRotation(bool);
    };
};
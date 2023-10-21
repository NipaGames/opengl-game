#pragma once

#include "../physics.h"
#include <core/entity/component.h>

namespace Physics {
    enum class ColliderConstructor {
        TRANSFORM,
        AABB,
        MESH
    };

    class RigidBody : public Component<RigidBody> {
    private:
        bool enableDebugVisualization_ = true;
        bool enableRotation_ = true;
        glm::vec3 colliderOriginOffset_ = glm::vec3(0.0f);
    public:
        btRigidBody* rigidBody = nullptr;
        DEFINE_COMPONENT_DATA_VALUE(btCollisionShape*, collider, nullptr);
        DEFINE_COMPONENT_DATA_VALUE(ColliderConstructor, colliderFrom, ColliderConstructor::TRANSFORM);
        DEFINE_COMPONENT_DATA_VALUE(float, mass, 1.0f);
        DEFINE_COMPONENT_DATA_VALUE(bool, interpolate, false);
        DEFINE_COMPONENT_DATA_VALUE(bool, doesMassAffectGravity, false);
        ~RigidBody();
        void Start();
        void Update();
        void FixedUpdate();
        void SetPos(const glm::vec3&);
        void UpdateTransform();
        void EnableDebugVisualization(bool);
        void EnableRotation(bool);
    };
};
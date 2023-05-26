#pragma once

#include <core/physics/physics.h>
#include <core/entity/component.h>

namespace Physics {
    class RigidBody : public Component<RigidBody> {
    private:
        bool enableDebugVisualization_ = true;
        bool enableRotation_ = true;
    public:
        btCollisionShape* collider = nullptr;
        btRigidBody* rigidBody = nullptr;
        DEFINE_COMPONENT_DATA_VALUE(float, mass, 1.0f);
        DEFINE_COMPONENT_DATA_VALUE(bool, interpolate, false);
        ~RigidBody();
        void Start();
        void Update();
        void FixedUpdate();
        void SetPos(const glm::vec3&);
        void UpdateTransform();
        bool IsGrounded(float = .05f);
        void EnableDebugVisualization(bool);
        void EnableRotation(bool);
    };
};
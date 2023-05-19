#pragma once

#include <core/physics/physics.h>
#include <core/entity/component.h>

namespace Physics {
    class RigidBody : public Component<RigidBody> {
    public:
        btCollisionShape* collider = nullptr;
        btRigidBody* rigidBody = nullptr;
        DEFINE_COMPONENT_DATA_VALUE(float, mass, 1.0f);
        ~RigidBody();
        void Start();
        void FixedUpdate();
    };
};
#include "rigidbody.h"

#include "../utils.h"
#include <core/entity/entity.h>
#include <core/graphics/component/meshrenderer.h>

using namespace Physics;

RigidBody::~RigidBody() {
    if (rigidBody != nullptr)
        dynamicsWorld->removeRigidBody(rigidBody);
    delete collider;
}

void RigidBody::Start() {
    Transform* t = parent->transform;
    btTransform transform;
    transform.setIdentity();

    MeshRenderer* meshRenderer;
    if (collider == nullptr) {
        switch (colliderFrom) {
            case ColliderConstructor::TRANSFORM:
                collider = new btBoxShape(btVector3(.5f, .5f, .5f));
                break;
            case ColliderConstructor::AABB:
                meshRenderer = parent->GetComponent<MeshRenderer>();
                collider = new btBoxShape(Physics::GLMVectorToBtVector3(meshRenderer->GetAABB().extents));
                colliderOriginOffset_ = meshRenderer->GetAABB().center;
                transform.setOrigin(Physics::GLMVectorToBtVector3(colliderOriginOffset_ * t->size));
                break;
            case ColliderConstructor::MESH:
                break;
        }
    }
    collider->setLocalScaling(collider->getLocalScaling() * Physics::GLMVectorToBtVector3(t->size));
    btVector3 localInertia(0, 0, 0);
    if (mass != 0.0f)
        collider->calculateLocalInertia(mass, localInertia);

    transform.setOrigin(transform.getOrigin() + Physics::GLMVectorToBtVector3(t->position));
    transform.setRotation(Physics::GLMQuatToBtQuat(t->rotation));

    btDefaultMotionState* motionState = new btDefaultMotionState(transform);
    btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, collider, localInertia);
    rbInfo.m_restitution = 0.0f;
    rigidBody = new btRigidBody(rbInfo);
    dynamicsWorld->addRigidBody(rigidBody);
    EnableDebugVisualization(enableDebugVisualization_);
    EnableRotation(enableRotation_);
    rigidBody->setRestitution(0.0f);
    rigidBody->setDamping(0.0f, 1.0f);
    if (doesMassAffectGravity)
        rigidBody->setGravity(rigidBody->getGravity() * mass);
}

void RigidBody::UpdateTransform() {
    Transform* t = parent->transform;
    btTransform transform;
    rigidBody->getMotionState()->getWorldTransform(transform);

    t->position = Physics::BtVectorToGLMVector3(transform.getOrigin()) - colliderOriginOffset_ * t->size;
    t->rotation = Physics::BtQuatToGLMQuat(transform.getRotation());
}

void RigidBody::Update() {
    if (interpolate)
        UpdateTransform();
}

void RigidBody::FixedUpdate() {
    if (!interpolate)
        UpdateTransform();
    //rigidBody->getCollisionShape()->setLocalScaling(btVector3(t->size.x, t->size.y, t->size.z));
}

void RigidBody::SetPos(const glm::vec3& pos) {
    btTransform transform;
    btMotionState* ms = rigidBody->getMotionState();
    ms->getWorldTransform(transform);
    transform.setOrigin(btVector3(pos.x, pos.y, pos.z));
    ms->setWorldTransform(transform);
    rigidBody->setMotionState(ms);
    parent->transform->position = pos;
}

void RigidBody::EnableDebugVisualization(bool enabled) {
    enableDebugVisualization_ = enabled;
    if (rigidBody == nullptr)
        return;
    
    if (enableDebugVisualization_)
        rigidBody->setCollisionFlags(rigidBody->getCollisionFlags() & ~btCollisionObject::CF_DISABLE_VISUALIZE_OBJECT);
    else
        rigidBody->setCollisionFlags(rigidBody->getCollisionFlags() | btCollisionObject::CF_DISABLE_VISUALIZE_OBJECT);
}

void RigidBody::EnableRotation(bool enabled) {
    enableRotation_ = enabled;
    if (rigidBody == nullptr)
        return;
    rigidBody->setAngularFactor(1.0f * enableRotation_);
}


// serialization
#include <core/io/serializer.h>
#include <core/io/serializetypes.h>

JSON_SERIALIZE_TYPES([](Serializer::SerializationArgs& args, const nlohmann::json& j) {
    if (!j.is_string())
        return false;
    auto constructor = magic_enum::enum_cast<ColliderConstructor>((std::string) j);
    if (!constructor.has_value())
        return false;
    args.Return(constructor.value());
    return true;
}, ColliderConstructor);

// TODO: create colliders from scratch
JSON_SERIALIZE_TYPES([](Serializer::SerializationArgs& args, const nlohmann::json& j) {
    btCollisionShape* collider = nullptr;
    args.Return(collider);
    return true;
}, btCollisionShape*);
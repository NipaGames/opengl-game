#include "rigidbody.h"

#include <core/entity/entity.h>

using namespace Physics;

RigidBody::~RigidBody() {
    if (rigidBody != nullptr)
        dynamicsWorld->removeRigidBody(rigidBody);
    delete collider;
}

void RigidBody::Start() {
    Transform* t = parent->transform;

    if (collider == nullptr) {
        collider = new btBoxShape(btVector3(.5f, .5f, .5f));
    }
    collider->setLocalScaling(collider->getLocalScaling() * btVector3(t->size.x, t->size.y, t->size.z));
    btVector3 localInertia(0, 0, 0);
    if (mass != 0.0f)
        collider->calculateLocalInertia(mass, localInertia);

    btTransform transform;
    transform.setIdentity();
    transform.setOrigin({ t->position.x, t->position.y, t->position.z });

    btDefaultMotionState* motionState = new btDefaultMotionState(transform);
    btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, collider, localInertia);
    rigidBody = new btRigidBody(rbInfo);
    dynamicsWorld->addRigidBody(rigidBody);
}

void RigidBody::FixedUpdate() {
    Transform* t = parent->transform;
    btTransform transform;
    rigidBody->getMotionState()->getWorldTransform(transform);

    t->position.x = transform.getOrigin().getX();
    t->position.y = transform.getOrigin().getY();
    t->position.z = transform.getOrigin().getZ();

    t->rotation.x = transform.getRotation().getX();
    t->rotation.y = transform.getRotation().getY();
    t->rotation.z = transform.getRotation().getZ();
    t->rotation.w = transform.getRotation().getW();

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

bool RigidBody::IsGrounded(float dist) {
    btVector3 rayFrom(parent->transform->position.x, parent->transform->position.y, parent->transform->position.z);
    btVector3 rayTo = rayFrom;
    rayTo.setY(rayTo.getY() - dist);
    btCollisionWorld::ClosestRayResultCallback res(rayFrom, rayTo);
    Physics::dynamicsWorld->rayTest(rayFrom, rayTo, res);
    return res.hasHit();
}
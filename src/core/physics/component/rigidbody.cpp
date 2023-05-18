#include "core/physics/component/rigidbody.h"
#include "core/entity/entity.h"

using namespace Physics;

RigidBody::~RigidBody() {
    if (rigidBody != nullptr)
        dynamicsWorld->removeRigidBody(rigidBody);
}

void RigidBody::Start() {
    Transform* t = parent->transform;
    btCollisionShape* colShape = new btBoxShape({ t->size.x, t->size.y, t->size.z });
    collisionShapes.push_back(colShape);

    btVector3 localInertia(0, 0, 0);
    if (mass != 0.0f)
        colShape->calculateLocalInertia(mass, localInertia);

    btTransform transform;
    transform.setIdentity();
    transform.setOrigin({ t->position.x, t->position.y, t->position.z });

    btDefaultMotionState* motionState = new btDefaultMotionState(transform);
    btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, colShape, localInertia);
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
}
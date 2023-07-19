#include "transform.h"

#include <core/physics/utils.h>

btVector3 Transform::btGetPos() {
    return Physics::GLMVectorToBtVector3(position);
}

btVector3 Transform::btGetSize() {
    return Physics::GLMVectorToBtVector3(size);
}

btQuaternion Transform::btGetRotation() {
    return Physics::GLMQuatToBtQuat(rotation);
}

btTransform Transform::btGetTransform() {
    btTransform trans;
    trans.setIdentity();
    trans.setOrigin(btGetPos());
    trans.setRotation(btGetRotation());
    return trans;
}

void Transform::btSetTransform(const btTransform& trans) {
    position = Physics::BtVectorToGLMVector3(trans.getOrigin());
    rotation = Physics::BtQuatToGLMQuat(trans.getRotation());
}
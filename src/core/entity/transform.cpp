#include "transform.h"

btVector3 Transform::btGetPos() {
    return btVector3(position.x, position.y, position.z);
}

btVector3 Transform::btGetSize() {
    return btVector3(size.x, size.y, size.z);
}

btQuaternion Transform::btGetRotation() {
    btQuaternion quat;
    quat.setEulerZYX(rotation.x, rotation.y, rotation.z);
    return quat;
}

btTransform Transform::btGetTransform() {
    btTransform trans;
    trans.setIdentity();
    trans.setOrigin(btGetPos());
    trans.setRotation(btGetRotation());
    return trans;
}

void Transform::btSetTransform(const btTransform& trans) {
    const btVector3& pos = trans.getOrigin();
    position = { pos.getX(), pos.getY(), pos.getZ() };
    const btQuaternion& rot = trans.getRotation();
    rotation = { rot.getX(), rot.getY(), rot.getZ(), rot.getW() };
}
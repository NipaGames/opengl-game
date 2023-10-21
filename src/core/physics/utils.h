#pragma once

#include "physics.h"

namespace Physics {
    inline btVector3 GLMVectorToBtVector3(const glm::vec3& vec) {
        return btVector3(vec.x, vec.y, vec.z);
    }
    inline glm::vec3 BtVectorToGLMVector3(const btVector3& vec) {
        return glm::vec3(vec.getX(), vec.getY(), vec.getZ());
    }
    inline btQuaternion GLMVectorToBtQuat(const glm::vec3& quat) {
        btQuaternion btQuat;
        btQuat.setEulerZYX(quat.z, quat.y, quat.x);
        return btQuat;
    }
    inline btQuaternion GLMQuatToBtQuat(const glm::quat& quat) {
        return GLMVectorToBtQuat(glm::eulerAngles(quat));
    }
    inline glm::quat BtQuatToGLMQuat(const btQuaternion& quat) {
        return glm::quat(quat.getW(), quat.getX(), quat.getY(), quat.getZ());
    }
};
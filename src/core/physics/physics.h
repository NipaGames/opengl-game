#pragma once

#include <bullet/btBulletDynamicsCommon.h>

namespace Physics {
    // don't really know if these have a reason to be heap-allocated but the bullet HelloWorld.cpp has theme allocated like this,
    // so there must be some reason, right?
    inline btDefaultCollisionConfiguration* collisionConfiguration = nullptr;
	inline btCollisionDispatcher* dispatcher = nullptr;
	inline btBroadphaseInterface* overlappingPairCache = nullptr;
	inline btSequentialImpulseConstraintSolver* solver = nullptr;
	inline btDiscreteDynamicsWorld* dynamicsWorld = nullptr;
    void Init();
    void Destroy();
};
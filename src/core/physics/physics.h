#pragma once

#include <bullet/btBulletDynamicsCommon.h>
#include "core/physics/debugdrawer.h"

namespace Physics {
    // don't really know if these have a reason to be heap-allocated but the official Bullet HelloWorld.cpp has them allocated like this,
    // so there must be some reason, right?
    inline btDefaultCollisionConfiguration* collisionConfiguration = nullptr;
	inline btCollisionDispatcher* dispatcher = nullptr;
	inline btBroadphaseInterface* overlappingPairCache = nullptr;
	inline btSequentialImpulseConstraintSolver* solver = nullptr;
	inline btDiscreteDynamicsWorld* dynamicsWorld = nullptr;
    inline DebugDrawer* debugDrawer = nullptr;
    inline btAlignedObjectArray<btCollisionShape*> collisionShapes;
    void Init();
    void Destroy();
    void Update(double);
};
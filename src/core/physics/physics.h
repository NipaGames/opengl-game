#pragma once

#include <btBulletDynamicsCommon.h>
#include "debugdrawer.h"

namespace Physics {
    // don't really know if these have a reason to be heap-allocated but the official Bullet HelloWorld.cpp has them allocated like this,
    // so there must be some reason, right?
    inline btVector3 worldSize = btVector3(2000, 2000, 2000);
    inline btDefaultCollisionConfiguration* collisionConfiguration = nullptr;
	inline btCollisionDispatcher* dispatcher = nullptr;
	inline btAxisSweep3* axisSweep = nullptr;
	inline btSequentialImpulseConstraintSolver* solver = nullptr;
	inline btDiscreteDynamicsWorld* dynamicsWorld = nullptr;
    inline DebugDrawer* debugDrawer = nullptr;
    void Init();
    void Destroy();
    void Update(double);
};
#include "physics.h"
#include <core/game.h>

void Physics::Init() {
    collisionConfiguration = new btDefaultCollisionConfiguration();
    dispatcher = new btCollisionDispatcher(collisionConfiguration);
    overlappingPairCache = new btDbvtBroadphase();
    solver = new btSequentialImpulseConstraintSolver;
    dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);
    dynamicsWorld->setGravity(btVector3(0, -10, 0));

    debugDrawer = new DebugDrawer();
    dynamicsWorld->setDebugDrawer(debugDrawer);
}

void Physics::Update(double dt) {
    dynamicsWorld->stepSimulation(btScalar(dt));
}

void Physics::Destroy() {
    delete dynamicsWorld;
    delete solver;
    delete overlappingPairCache;
    delete dispatcher;
    delete collisionConfiguration;
    delete debugDrawer;
}
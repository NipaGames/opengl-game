#include "core/physics/physics.h"
#include "core/game.h"

void Physics::Init() {
    collisionConfiguration = new btDefaultCollisionConfiguration();
    dispatcher = new btCollisionDispatcher(collisionConfiguration);
    overlappingPairCache = new btDbvtBroadphase();
    solver = new btSequentialImpulseConstraintSolver;
    dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);
    dynamicsWorld->setGravity(btVector3(0, -10, 0));

    debugDrawer = new DebugDrawer();
    debugDrawer->setDebugMode(btIDebugDraw::DBG_DrawWireframe);
    dynamicsWorld->setDebugDrawer(debugDrawer);

    btCollisionShape* colShape = new btSphereShape(btScalar(1.));
    collisionShapes.push_back(colShape);
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
    collisionShapes.clear();
}
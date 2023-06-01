#include "physics.h"
#include <core/game.h>

void Physics::Init() {
    collisionConfiguration = new btDefaultCollisionConfiguration();
    dispatcher = new btCollisionDispatcher(collisionConfiguration);
    axisSweep = new btAxisSweep3(-worldSize / 2, worldSize / 2);
    solver = new btSequentialImpulseConstraintSolver;
    dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, axisSweep, solver, collisionConfiguration);
    dynamicsWorld->setGravity(btVector3(0, -10, 0));

    debugDrawer = new DebugDrawer();
    dynamicsWorld->setDebugDrawer(debugDrawer);
}

void Physics::Update(double dt) {
    dynamicsWorld->stepSimulation(btScalar(dt), 10, btScalar(game->GetFixedDeltaTime()));
}

void Physics::Destroy() {
    delete dynamicsWorld;
    delete solver;
    delete axisSweep;
    delete dispatcher;
    delete collisionConfiguration;
    delete debugDrawer;
}
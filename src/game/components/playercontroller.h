#pragma once

#include <opengl.h>
#include <core/entity/component.h>
#include <core/physics/component/rigidbody.h>

class PlayerController : public Component<PlayerController> {
private:
    void OnMouseMove();
    bool isFlying_ = false;
    Physics::RigidBody* rigidBody_ = nullptr;
public:
    void Spawn();

    void Start();
    void Update();

    DEFINE_COMPONENT_DATA_VALUE(int, speed, 10);
    DEFINE_COMPONENT_DATA_VALUE(glm::vec3, spawnPosition, glm::vec3(0.0f));
    DEFINE_COMPONENT_DATA_VALUE_VECTOR(int, testNums);
    float mass = 4;
    glm::vec3 velocity;
};
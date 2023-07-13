#pragma once

#include <opengl.h>
#include <core/entity/component.h>
#include <core/physics/component/rigidbody.h>
#include <BulletDynamics/Character/btKinematicCharacterController.h>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>

class PlayerController : public Component<PlayerController> {
private:
    void OnMouseMove();
    bool isFlying_ = false;
    btKinematicCharacterController* characterController_ = nullptr;
    btPairCachingGhostObject* ghostObject_ = nullptr;
    float stepHeight_ = 0.2f;
    float gravity_ = -9.8f * 3.0f;
    float jumpSpeed_ = 10.0f;
    bool pushRigidBodies_ = false;
public:
    ~PlayerController();
    void Spawn();

    void Start();
    void Update();

    DEFINE_COMPONENT_DATA_VALUE(float, speed, 10.0f);
    DEFINE_COMPONENT_DATA_VALUE(glm::vec3, spawnPosition, glm::vec3(0.0f));
    DEFINE_COMPONENT_DATA_VALUE_VECTOR(int, testNums);
    float mass = 4;
};
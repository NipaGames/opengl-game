#pragma once

#include <opengl.h>
#include <core/entity/component.h>
#include <core/physics/component/rigidbody.h>
#include <core/entity/serializable.h>
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
    bool running_ = false;
public:
    ~PlayerController();
    void Spawn();

    void Start();
    void Update();

    SDEF(float, speed) = SVAL(10.0f);
    SDEF(glm::vec3, spawnPosition) = SVAL(glm::vec3(0.0f));
    SDEFVD(int, testNums);
};
#include "rigidbody.h"

#include "../utils.h"
#include <core/entity/entity.h>
#include <core/graphics/component/meshrenderer.h>

using namespace Physics;

RigidBody::~RigidBody() {
    if (rigidBody != nullptr)
        dynamicsWorld->removeRigidBody(rigidBody);
    if (meshData_ != nullptr) {
        for (int i = 0; i < meshData_->getIndexedMeshArray().size(); i++) {
            delete[] meshData_->getIndexedMeshArray()[i].m_vertexBase;
            delete[] meshData_->getIndexedMeshArray()[i].m_triangleIndexBase;
        }
        delete meshData_;
    }
    delete collider;
}


// really not feeling it right now, just copied from here
// https://pybullet.org/Bullet/phpBB3/viewtopic.php?t=7802
// (this even comes with 16 bit optimizations!!!)
btCollisionShape* RigidBody::CreateMeshCollider() {
    meshData_ = new btTriangleIndexVertexArray();
    const std::vector<std::shared_ptr<Mesh>>& meshes = parent->GetComponent<MeshRenderer>()->meshes;
    for (int meshIndex = 0; meshIndex < meshes.size(); meshIndex++) {
        const std::shared_ptr<Mesh>& m = meshes.at(meshIndex);
        btIndexedMesh tempMesh;
        meshData_->addIndexedMesh(tempMesh, PHY_FLOAT);

        btIndexedMesh& mesh = meshData_->getIndexedMeshArray()[meshIndex];

        size_t numIndices = m->indices.size();
        mesh.m_numTriangles = (int) numIndices / 3;
        
        if (numIndices < std::numeric_limits<int16_t>::max()) {
            // we can use 16-bit indices
            mesh.m_triangleIndexBase = new unsigned char[sizeof(int16_t) * (size_t) numIndices];
            mesh.m_indexType = PHY_SHORT;
            mesh.m_triangleIndexStride = 3 * sizeof(int16_t);
        } else {
            // we need 32-bit indices
            mesh.m_triangleIndexBase = new unsigned char[sizeof(int32_t) * (size_t) numIndices];
            mesh.m_indexType = PHY_INTEGER;
            mesh.m_triangleIndexStride = 3 * sizeof(int32_t);
        }

        mesh.m_numVertices = (int) m->vertices.size();
        mesh.m_vertexBase = new unsigned char[3 * sizeof(btScalar) * (size_t) mesh.m_numVertices];
        mesh.m_vertexStride = 3 * sizeof(btScalar);

        // copy vertices into mesh
        btScalar* vertexData = static_cast<btScalar*>((void*)(mesh.m_vertexBase));
        for (int32_t i = 0; i < mesh.m_numVertices * 3; i++) {
            vertexData[i] = m->vertices[i];
        }
        // copy indices into mesh
        if (numIndices < std::numeric_limits<int16_t>::max()) {
            // 16-bit case
            int16_t* indices = static_cast<int16_t*>((void*) (mesh.m_triangleIndexBase));
            for (int32_t i = 0; i < numIndices; i++) {
                indices[i] = (int16_t) m->indices[i];
            }
        } else {
            // 32-bit case
            int32_t* indices = static_cast<int32_t*>((void*) (mesh.m_triangleIndexBase));
            for (int32_t i = 0; i < numIndices; i++) {
                indices[i] = m->indices[i];
            }
        }
    }
    btBvhTriangleMeshShape* colliderShape = new btBvhTriangleMeshShape(meshData_, true);
    return colliderShape;
}

void RigidBody::Start() {
    Transform* t = parent->transform;
    btTransform transform;
    transform.setIdentity();

    MeshRenderer* meshRenderer;
    if (collider == nullptr) {
        switch (colliderFrom) {
            case ColliderConstructor::TRANSFORM:
                collider = new btBoxShape(btVector3(.5f, .5f, .5f));
                break;
            case ColliderConstructor::AABB:
                meshRenderer = parent->GetComponent<MeshRenderer>();
                collider = new btBoxShape(Physics::GLMVectorToBtVector3(meshRenderer->GetAABB().extents));
                colliderOriginOffset_ = meshRenderer->GetAABB().center;
                transform.setOrigin(Physics::GLMVectorToBtVector3(colliderOriginOffset_ * t->size));
                break;
            case ColliderConstructor::MESH:
                collider = CreateMeshCollider();
                break;
        }
    }
    collider->setLocalScaling(collider->getLocalScaling() * Physics::GLMVectorToBtVector3(t->size));
    btVector3 localInertia(0, 0, 0);
    if (mass != 0.0f)
        collider->calculateLocalInertia(mass, localInertia);

    transform.setOrigin(transform.getOrigin() + Physics::GLMVectorToBtVector3(t->position));
    transform.setRotation(Physics::GLMQuatToBtQuat(t->rotation));

    btDefaultMotionState* motionState = new btDefaultMotionState(transform);
    btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, collider, localInertia);
    rbInfo.m_restitution = 0.0f;
    rigidBody = new btRigidBody(rbInfo);
    dynamicsWorld->addRigidBody(rigidBody);
    EnableDebugVisualization(enableDebugVisualization_);
    EnableRotation(enableRotation_);
    rigidBody->setRestitution(0.0f);
    rigidBody->setDamping(0.0f, 1.0f);
    if (doesMassAffectGravity)
        rigidBody->setGravity(rigidBody->getGravity() * mass);
}

void RigidBody::UpdateTransform() {
    Transform* t = parent->transform;
    btTransform transform;
    rigidBody->getMotionState()->getWorldTransform(transform);

    t->position = Physics::BtVectorToGLMVector3(transform.getOrigin()) - colliderOriginOffset_ * t->size;
    t->rotation = Physics::BtQuatToGLMQuat(transform.getRotation());
}

void RigidBody::CopyTransform() {
    Transform* t = parent->transform;
    btTransform transform;
    btMotionState* ms = rigidBody->getMotionState();
    ms->getWorldTransform(transform);
    
    transform.setOrigin(Physics::GLMVectorToBtVector3(t->position + colliderOriginOffset_ * t->size));
    transform.setRotation(t->btGetRotation());

    ms->setWorldTransform(transform);
    rigidBody->setMotionState(ms);
}

void RigidBody::Update() {
    if (interpolate) {
        if (overwriteTransform)
            UpdateTransform();
        else
            CopyTransform();
    }
}

void RigidBody::FixedUpdate() {
    if (!interpolate) {
        if (overwriteTransform)
            UpdateTransform();
        else
            CopyTransform();
    }
    //rigidBody->getCollisionShape()->setLocalScaling(btVector3(t->size.x, t->size.y, t->size.z));
}

void RigidBody::SetPos(const glm::vec3& pos) {
    btTransform transform;
    btMotionState* ms = rigidBody->getMotionState();
    ms->getWorldTransform(transform);
    transform.setOrigin(btVector3(pos.x, pos.y, pos.z));
    ms->setWorldTransform(transform);
    rigidBody->setMotionState(ms);
    parent->transform->position = pos;
}

void RigidBody::EnableDebugVisualization(bool enabled) {
    enableDebugVisualization_ = enabled;
    if (rigidBody == nullptr)
        return;
    
    if (enableDebugVisualization_)
        rigidBody->setCollisionFlags(rigidBody->getCollisionFlags() & ~btCollisionObject::CF_DISABLE_VISUALIZE_OBJECT);
    else
        rigidBody->setCollisionFlags(rigidBody->getCollisionFlags() | btCollisionObject::CF_DISABLE_VISUALIZE_OBJECT);
}

void RigidBody::EnableRotation(bool enabled) {
    enableRotation_ = enabled;
    if (rigidBody == nullptr)
        return;
    rigidBody->setAngularFactor(1.0f * enableRotation_);
}


// serialization
#include <core/io/serializer.h>
#include <core/io/serializetypes.h>

JSON_SERIALIZE_ENUM(ColliderConstructor);

// TODO: create colliders from scratch
JSON_SERIALIZE_TYPES([](Serializer::SerializationArgs& args, const nlohmann::json& j) {
    btCollisionShape* collider = nullptr;
    args.Return(collider);
    return true;
}, btCollisionShape*);
#pragma once

#include <glm/gtc/matrix_transform.hpp>

class ViewFrustum {
public:
    struct AABB {
        glm::vec3 center = glm::vec3(0.0f);
        glm::vec3 extents = glm::vec3(.5f);
        static AABB FromMinMax(const glm::vec3& min, const glm::vec3& max) {
            AABB aabb;
            aabb.center = (max + min) * 0.5f;
            aabb.extents = max - aabb.center;
            return aabb;
        }
        glm::vec3 GetMin() {
            return center - extents;
        }
        glm::vec3 GetMax() {
            return center + extents;
        }
    };
    struct FrustumPlane {
        glm::vec3 normal = { 0.0f, 1.0f, 0.0f };
        float dist = 0.0f;
        FrustumPlane(const glm::vec3& p, const glm::vec3& norm) : normal(glm::normalize(norm)), dist(glm::dot(normal, p)) { }
        FrustumPlane() = default;
    };
    FrustumPlane top, bottom, left, right;
    FrustumPlane nearClippingPlane, farClippingPlane;
    bool IsOnPlane(const AABB&, const FrustumPlane&) const;
    bool IsOnFrustum(const AABB&) const;
};

class Camera {
public:
    ViewFrustum frustum;
    float fov = 60.0f;
    float yaw = 90.0f;
    float pitch = 0.0f;
    float clippingFar = 1000.0f;
    float clippingNear = .1f;
    float aspectRatio;
    glm::vec3 front = glm::vec3(0.0f, 0.0f, 1.0f);
    glm::vec3 right = glm::vec3(1.0f, 0.0f, 0.0f);
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 pos = glm::vec3(0.0f);
    glm::mat4 projectionMatrix;
    void UpdateFrustum();
};
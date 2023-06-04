#pragma once

#include <glm/gtc/matrix_transform.hpp>

class ViewFrustum {
public:
    struct AABB {
        glm::vec3 center = glm::vec3(0.0f);
        glm::vec3 extents = glm::vec3(0.0f);
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
    float clippingFar = 100.0f;
    float clippingNear = .1f;
    float aspectRatio;
    glm::vec3 front = glm::vec3(0.0f, 0.0f, 1.0f);
    glm::vec3 right = glm::vec3(1.0f, 0.0f, 0.0f);
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 pos = glm::vec3(0.0f);
    glm::mat4 projectionMatrix;
    void UpdateFrustum();
};
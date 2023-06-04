#include "camera.h"

bool ViewFrustum::IsOnPlane(const AABB& aabb, const FrustumPlane& plane) const {
    const float r =
        aabb.extents.x * std::abs(plane.normal.x) +
        aabb.extents.y * std::abs(plane.normal.y) +
	    aabb.extents.z * std::abs(plane.normal.z);

		return -r <= glm::dot(plane.normal, aabb.center) - plane.dist;
}

bool ViewFrustum::IsOnFrustum(const AABB& aabb) const {
    return (
        IsOnPlane(aabb, left) &&
        IsOnPlane(aabb, right) &&
        IsOnPlane(aabb, top) &&
        IsOnPlane(aabb, bottom) &&
        IsOnPlane(aabb, nearClippingPlane) &&
        IsOnPlane(aabb, farClippingPlane)
    );
}

void Camera::UpdateFrustum() {
    glm::vec3 perspectiveUp = glm::normalize(glm::cross(right, front));
    float halfVSide = clippingFar * glm::tan(glm::radians(fov) * .5f);
    float halfHSide = halfVSide * aspectRatio;
    glm::vec3 frontMultFar = clippingFar * front;

    frustum.nearClippingPlane = { pos + clippingNear * front, front };
    frustum.farClippingPlane = { pos + frontMultFar, -front };
    frustum.right = { pos, glm::cross(frontMultFar - right * halfHSide, perspectiveUp) };
    frustum.left = { pos, glm::cross(perspectiveUp, frontMultFar + right * halfHSide) };
    frustum.top = { pos, glm::cross(right, frontMultFar - perspectiveUp * halfVSide) };
    frustum.bottom = { pos, glm::cross(frontMultFar + perspectiveUp * halfVSide, right) };
}
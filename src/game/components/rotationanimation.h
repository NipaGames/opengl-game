#pragma once
#include <core/entity/component.h>
#include <core/graphics/component/meshrenderer.h>
#include <opengl.h>

class RotationAnimationComponent : public Component<RotationAnimationComponent> {
private:
    bool isPlaying_ = false;
    float animationStart_;
    glm::mat4 startTransform_;
    MeshRenderer* meshRenderer_;
public:
    DEFINE_COMPONENT_DATA_VALUE(float, animationLength, 1.0);
    DEFINE_COMPONENT_DATA_VALUE(float, animationDelay, 0.0);
    // start and negative end angle
    DEFINE_COMPONENT_DATA_VALUE(float, angle, 60.0f);
    DEFINE_COMPONENT_DATA_VALUE(glm::vec3, rotationAxis, glm::vec3(0.0f, 0.0f, 1.0f));
    DEFINE_COMPONENT_DATA_VALUE_VECTOR(int, meshTransforms);
    DEFINE_COMPONENT_DATA_VALUE(bool, transformAtStart, true);
    static void RotationAnimation(std::string);
    void Play();
    void Start();
    void Update();
};
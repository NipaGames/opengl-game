#pragma once
#include <core/entity/component.h>
#include <core/graphics/component/meshrenderer.h>
#include <opengl.h>

class AnimationComponent : public Component<AnimationComponent> {
private:
    bool isPlaying_ = false;
    float animationStart_;
public:
    DEFINE_COMPONENT_DATA_VALUE(float, animationLength, 1.0f);
    DEFINE_COMPONENT_DATA_VALUE(float, animationDelay, 0.0f);
    DEFINE_COMPONENT_DATA_VALUE(bool, playReverse, false);
    DEFINE_COMPONENT_DATA_VALUE(bool, allowInterruptions, false);
    // start and negative end angle
    static void PlayAnimation(std::string, std::string);
    virtual void Interpolate(float) { }
    virtual void Play();
    virtual void Start();
    virtual void Update();
};

class MeshTransformAnimation : public AnimationComponent {
protected:
    glm::mat4 baseTransform_;
    MeshRenderer* meshRenderer_ = nullptr;
public:
    DEFINE_COMPONENT_DATA_VALUE_VECTOR(int, meshTransforms);
    DEFINE_COMPONENT_DATA_VALUE(bool, transformAtStart, true);

    virtual glm::mat4 Transform(float) { return baseTransform_; }
    void Interpolate(float) override;
    void UpdateTransforms(const glm::mat4&);
    void Play() override;
    void Update() override;
};
REGISTER_COMPONENT(MeshTransformAnimation);

class MeshRotationAnimation : public MeshTransformAnimation {
public:
    DEFINE_COMPONENT_DATA_VALUE(float, angle, 60.0f);
    DEFINE_COMPONENT_DATA_VALUE(glm::vec3, rotationAxis, glm::vec3(0.0f, 0.0f, 1.0f));
    glm::mat4 Transform(float) override;
};
REGISTER_COMPONENT(MeshRotationAnimation);

class FloatAnimation : public AnimationComponent {
public:
    float* ptr = nullptr;
    void Interpolate(float) override;
};
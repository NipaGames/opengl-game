#pragma once

#include <opengl.h>
#include <core/entity/component.h>
#include <core/entity/entity.h>

namespace Light {
    inline int POINT_LIGHTS_INDEX = 0;
    inline int DIRECTIONAL_LIGHTS_INDEX = 0;
    inline int SPOTLIGHTS_INDEX = 0;

    inline void ResetIndices() {
        POINT_LIGHTS_INDEX = 0;
        DIRECTIONAL_LIGHTS_INDEX = 0;
        SPOTLIGHTS_INDEX = 0;
    }

    enum class LightType {
        POINT,
        DIRECTIONAL,
        SPOTLIGHT,
        NONE
    };

    class Light : public Component<Light> {
    private:
        LightType type_ = LightType::NONE;
    protected:
        void SetType(LightType t) { type_ = t; }
    public:
        DEFINE_COMPONENT_DATA_VALUE(glm::vec3, color, glm::vec3(1.0f));
        DEFINE_COMPONENT_DATA_VALUE(float, intensity, 1.0f);

        bool isAdded = false;
        virtual ~Light();
        IComponent* Clone() const override;
        void Start() override;
        LightType GetType() { return type_; }
        virtual void ApplyLight(GLuint) const { }
    };

    class PointLight: public Light {
    public:
        DEFINE_COMPONENT_DATA_VALUE(float, range, 20.0f);

        PointLight() { SetType(LightType::POINT); }
        void ApplyLight(GLuint) const override;
    };
    REGISTER_COMPONENT(PointLight);

    class DirectionalLight : public Light {
    public:
        DEFINE_COMPONENT_DATA_VALUE_DEFAULT(glm::vec3, dir);
        
        DirectionalLight() { SetType(LightType::DIRECTIONAL); }
        void ApplyLight(GLuint) const override;
    };
    REGISTER_COMPONENT(DirectionalLight);

    class Spotlight : public Light {
    public:
        DEFINE_COMPONENT_DATA_VALUE_DEFAULT(float, cutOffMin);
        DEFINE_COMPONENT_DATA_VALUE_DEFAULT(float, cutOffMax);
        DEFINE_COMPONENT_DATA_VALUE_DEFAULT(glm::vec3, dir);
        DEFINE_COMPONENT_DATA_VALUE(float, range, 20.0f);

        Spotlight() { Light::SetType(LightType::SPOTLIGHT); }
        void ApplyLight(GLuint) const override;
    };
    REGISTER_COMPONENT(Spotlight);
}
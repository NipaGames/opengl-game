#pragma once

#include <opengl.h>
#include <core/entity/component.h>
#include <core/entity/entity.h>

namespace Lights {
    enum class LightType {
        NONE = 0,
        POINT = 1,
        SPOTLIGHT = 2,
        DIRECTIONAL = 3,
        DIRECTIONAL_PLANE = 4
    };
    
    inline int LIGHTS_INDEX = 0;
    const inline int MAX_LIGHTS = 32;

    // this is fucking evil right here
    // 32 lights, 1 bit for reserved, 0 for not
    inline uint32_t RESERVED_LIGHTS = 0x0;

    void ReserveIndex(int);
    bool IsReserved(int);
    int& GetIndex(LightType);
    void ResetIndices();

    class Light : public Component<Light> {
    private:
        LightType type_ = LightType::NONE;
    protected:
        void SetType(LightType t) { type_ = t; }
        std::string lightUniform_;
    public:
        DEFINE_COMPONENT_DATA_VALUE(glm::vec3, color, glm::vec3(1.0f));
        DEFINE_COMPONENT_DATA_VALUE(float, intensity, 1.0f);

        bool isAdded = false;
        virtual ~Light();
        IComponent* Clone() const override;
        void Start() override;
        LightType GetType() { return type_; }
        virtual void UseAsNext();
        virtual void ApplyLight(GLuint) const;
        virtual void ApplyForAllShaders() const;
    };

    class PointLight : public Light {
    public:
        DEFINE_COMPONENT_DATA_VALUE(float, range, 20.0f);

        PointLight() { Light::SetType(LightType::POINT); }
        void ApplyLight(GLuint) const;
    };
    REGISTER_COMPONENT(PointLight);

    class DirectionalLight : public Light {
    public:
        DEFINE_COMPONENT_DATA_VALUE_DEFAULT(glm::vec3, dir);
        
        DirectionalLight() { Light::SetType(LightType::DIRECTIONAL); }
        void ApplyLight(GLuint) const;
    };
    REGISTER_COMPONENT(DirectionalLight);

    class DirectionalLightPlane : public Light {
    public:
        DEFINE_COMPONENT_DATA_VALUE_DEFAULT(glm::vec3, dir);
        DEFINE_COMPONENT_DATA_VALUE(float, range, 20.0f);
        
        DirectionalLightPlane() { Light::SetType(LightType::DIRECTIONAL_PLANE); }
        void ApplyLight(GLuint) const;
    };
    REGISTER_COMPONENT(DirectionalLightPlane);

    class Spotlight : public Light {
    public:
        DEFINE_COMPONENT_DATA_VALUE_DEFAULT(float, cutOffMin);
        DEFINE_COMPONENT_DATA_VALUE_DEFAULT(float, cutOffMax);
        DEFINE_COMPONENT_DATA_VALUE_DEFAULT(glm::vec3, dir);
        DEFINE_COMPONENT_DATA_VALUE(float, range, 20.0f);

        Spotlight() { Light::SetType(LightType::SPOTLIGHT); }
        void ApplyLight(GLuint) const;
    };
    REGISTER_COMPONENT(Spotlight);
}
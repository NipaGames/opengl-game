#pragma once

#include <opengl.h>
#include <core/entity/component.h>
#include <core/entity/entity.h>

namespace Lights {
    enum class LightType {
        NONE,
        POINT,
        DIRECTIONAL,
        SPOTLIGHT
    };
    
    inline int POINT_LIGHTS_INDEX = 0;
    inline int DIRECTIONAL_LIGHTS_INDEX = 0;
    inline int SPOTLIGHTS_INDEX = 0;

    // this is fucking evil right here
    // we have max 8 lights, so divide this into 8 total 2 bit thingies
    // one 2 bit thingy contains the light enum (they take 2 bits,
    // will have to change if I add more light types)
    inline uint16_t RESERVED_LIGHTS = 0x0;

    void ReserveIndex(int, LightType);
    bool IsReserved(int, LightType);
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
        virtual void UseAsNext() { }
        virtual void ApplyLight(GLuint) const;
        virtual void ApplyForAllShaders() const;
    };

    class PointLight : public Light {
    public:
        DEFINE_COMPONENT_DATA_VALUE(float, range, 20.0f);

        PointLight() { Light::SetType(LightType::POINT); }
        void UseAsNext() override;
        void ApplyLight(GLuint) const;
    };
    REGISTER_COMPONENT(PointLight);

    class DirectionalLight : public Light {
    public:
        DEFINE_COMPONENT_DATA_VALUE_DEFAULT(glm::vec3, dir);
        
        DirectionalLight() { Light::SetType(LightType::DIRECTIONAL); }
        void UseAsNext() override;
        void ApplyLight(GLuint) const;
    };
    REGISTER_COMPONENT(DirectionalLight);

    class Spotlight : public Light {
    public:
        DEFINE_COMPONENT_DATA_VALUE_DEFAULT(float, cutOffMin);
        DEFINE_COMPONENT_DATA_VALUE_DEFAULT(float, cutOffMax);
        DEFINE_COMPONENT_DATA_VALUE_DEFAULT(glm::vec3, dir);
        DEFINE_COMPONENT_DATA_VALUE(float, range, 20.0f);

        Spotlight() { Light::SetType(LightType::SPOTLIGHT); }
        void UseAsNext() override;
        void ApplyLight(GLuint) const;
    };
    REGISTER_COMPONENT(Spotlight);
}
#pragma once

#include <opengl.h>
#include "core/entity/component.h"
#include "core/entity/entity.h"

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
        SPOTLIGHT
    };

    class Light : public Component<Light> {
    private:
        LightType type_;
    protected:
        void SetType(LightType t) { type_ = t; }
    public:
        DEFINE_COMPONENT_DATA_VALUE(glm::vec3, color, glm::vec3(1.0f));
        DEFINE_COMPONENT_DATA_VALUE(float, intensity, 1.0f);

        LightType GetType() { return type_; }
        virtual void ApplyLight(GLuint) { }
    };

    class PointLight: public Light {
    public:
        DEFINE_COMPONENT_DATA_VALUE(float, range, 20.0f);

        PointLight() { SetType(LightType::POINT); }
        void ApplyLight(GLuint shader) {
            std::string lightUniform = "pointLights[" + std::to_string(POINT_LIGHTS_INDEX++) + "]";
            glUniform3f(glGetUniformLocation(shader, std::string(lightUniform + ".pos").c_str()), parent->transform->position.x, parent->transform->position.y, parent->transform->position.z);
            glUniform1f(glGetUniformLocation(shader, std::string(lightUniform + ".range").c_str()), range);
            glUniform3f(glGetUniformLocation(shader, std::string(lightUniform + ".color").c_str()), color.x, color.y, color.z);
            glUniform1f(glGetUniformLocation(shader, std::string(lightUniform + ".intensity").c_str()), intensity);
        }
    };
    REGISTER_COMPONENT(PointLight);

    class DirectionalLight : public Light {
    public:
        DEFINE_COMPONENT_DATA_VALUE_DEFAULT(glm::vec3, dir);
        
        DirectionalLight() { SetType(LightType::DIRECTIONAL); }
        void ApplyLight(GLuint shader) {
            std::string lightUniform = "directionalLights[" + std::to_string(DIRECTIONAL_LIGHTS_INDEX++) + "]";
            glUniform3f(glGetUniformLocation(shader, std::string(lightUniform + ".dir").c_str()), dir.x, dir.y, dir.z);
            glUniform3f(glGetUniformLocation(shader, std::string(lightUniform + ".color").c_str()), color.x, color.y, color.z);
            glUniform1f(glGetUniformLocation(shader, std::string(lightUniform + ".intensity").c_str()), intensity);
        }
    };
    REGISTER_COMPONENT(DirectionalLight);

    class Spotlight : public Light {
    public:
        DEFINE_COMPONENT_DATA_VALUE_DEFAULT(float, cutOffMin);
        DEFINE_COMPONENT_DATA_VALUE_DEFAULT(float, cutOffMax);
        DEFINE_COMPONENT_DATA_VALUE_DEFAULT(glm::vec3, dir);
        DEFINE_COMPONENT_DATA_VALUE(float, range, 20.0f);

        Spotlight() { Light::SetType(LightType::SPOTLIGHT); }
        void ApplyLight(GLuint shader) {
            std::string lightUniform = "spotlightsights[" + std::to_string(POINT_LIGHTS_INDEX++) + "]";
            glUniform3f(glGetUniformLocation(shader, std::string(lightUniform + ".pos").c_str()), parent->transform->position.x, parent->transform->position.y, parent->transform->position.z);
            glUniform1f(glGetUniformLocation(shader, std::string(lightUniform + ".range").c_str()), range);
             glUniform1f(glGetUniformLocation(shader, std::string(lightUniform + ".cutOffMin").c_str()), cutOffMin);
            glUniform1f(glGetUniformLocation(shader, std::string(lightUniform + ".cutOffMax").c_str()), cutOffMax);
            glUniform3f(glGetUniformLocation(shader, std::string(lightUniform + ".color").c_str()), color.x, color.y, color.z);
            glUniform1f(glGetUniformLocation(shader, std::string(lightUniform + ".intensity").c_str()), intensity);
        }
    };
    REGISTER_COMPONENT(Spotlight);
}
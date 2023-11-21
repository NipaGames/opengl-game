#include "light.h"

#include <core/game.h>

using namespace Lights;

namespace Lights {
    void ReserveIndex(int index, LightType type) {
        RESERVED_LIGHTS |= (int) type << index * 2;
    }
    
    bool IsReserved(int i, LightType t) {
        return (Lights::RESERVED_LIGHTS >> i * 2) == (int) t;
    }

    void ResetIndices() {
        POINT_LIGHTS_INDEX = 0;
        DIRECTIONAL_LIGHTS_INDEX = 0;
        SPOTLIGHTS_INDEX = 0;
    }

    int LIGHT_NONE_INDEX = -1;
    int& GetIndex(LightType type) {
        switch (type) {
            case LightType::POINT:
                return POINT_LIGHTS_INDEX;
            case LightType::DIRECTIONAL:
                return DIRECTIONAL_LIGHTS_INDEX;
            case LightType::SPOTLIGHT:
                return SPOTLIGHTS_INDEX;
        }
        return LIGHT_NONE_INDEX;
    }

    Light::~Light() {
        if (isAdded)
            GAME->GetRenderer().RemoveLight(this);
    }

    void Light::Start() {
        if (!isAdded)
            GAME->GetRenderer().AddLight(this);
    }

    void Light::ApplyLight(GLuint shader) const {
        glUniform1i(glGetUniformLocation(shader, std::string(lightUniform_ + ".enabled").c_str()), GL_TRUE);
        glUniform3f(glGetUniformLocation(shader, std::string(lightUniform_ + ".color").c_str()), color.x, color.y, color.z);
        glUniform1f(glGetUniformLocation(shader, std::string(lightUniform_ + ".intensity").c_str()), intensity);
    }

    void Light::ApplyForAllShaders() const {
        for (GLuint shader : GAME->GetRenderer().GetShaders()) {
            glUseProgram(shader);
            ApplyLight(shader);
        }
        glUseProgram(0);
    }

    IComponent* Light::Clone() const {
        IComponent* c = nullptr;
        switch (type_) {
        case LightType::POINT:
            c = new PointLight(*static_cast<const PointLight*>(this));
            break;
        case LightType::DIRECTIONAL:
            c = new DirectionalLight(*static_cast<const DirectionalLight*>(this));
            break;
        case LightType::SPOTLIGHT:
            c = new Spotlight(*static_cast<const Spotlight*>(this));
            break;
        }
        return c;
    }

    void PointLight::UseAsNext() {
        lightUniform_ = "pointLights[" + std::to_string(POINT_LIGHTS_INDEX++) + "]";
    }
    
    void PointLight::ApplyLight(GLuint shader) const {
        Light::ApplyLight(shader);
        glUniform3f(glGetUniformLocation(shader, std::string(lightUniform_ + ".pos").c_str()), parent->transform->position.x, parent->transform->position.y, parent->transform->position.z);
        glUniform1f(glGetUniformLocation(shader, std::string(lightUniform_ + ".range").c_str()), range);
    }

    void DirectionalLight::UseAsNext() {
        lightUniform_ = "directionalLights[" + std::to_string(DIRECTIONAL_LIGHTS_INDEX++) + "]";
    }
    void DirectionalLight::ApplyLight(GLuint shader) const {
        Light::ApplyLight(shader);
        glUniform3f(glGetUniformLocation(shader, std::string(lightUniform_ + ".dir").c_str()), dir.x, dir.y, dir.z);
    }

    void Spotlight::UseAsNext() {
        lightUniform_ = "spotlights[" + std::to_string(SPOTLIGHTS_INDEX++) + "]";
    }
    void Spotlight::ApplyLight(GLuint shader) const {
        Light::ApplyLight(shader);
        glUniform3f(glGetUniformLocation(shader, std::string(lightUniform_ + ".dir").c_str()), dir.x, dir.y, dir.z);
        glUniform3f(glGetUniformLocation(shader, std::string(lightUniform_ + ".pos").c_str()), parent->transform->position.x, parent->transform->position.y, parent->transform->position.z);
        glUniform1f(glGetUniformLocation(shader, std::string(lightUniform_ + ".range").c_str()), range);
        glUniform1f(glGetUniformLocation(shader, std::string(lightUniform_ + ".cutOffMin").c_str()), cutOffMin);
        glUniform1f(glGetUniformLocation(shader, std::string(lightUniform_ + ".cutOffMax").c_str()), cutOffMax);
    }
};
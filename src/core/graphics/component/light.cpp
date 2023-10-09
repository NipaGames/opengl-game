#include "light.h"

#include <core/game.h>

using namespace Lights;

namespace Lights {
    Light::~Light() {
        if (isAdded)
            game->GetRenderer().RemoveLight(this);
    }

    void Light::Start() {
        if (!isAdded)
            game->GetRenderer().AddLight(this);
    }

    void Light::ApplyLight(GLuint shader) const {
        glUniform1i(glGetUniformLocation(shader, std::string(lightUniform_ + ".enabled").c_str()), GL_TRUE);
        glUniform3f(glGetUniformLocation(shader, std::string(lightUniform_ + ".color").c_str()), color.x, color.y, color.z);
        glUniform1f(glGetUniformLocation(shader, std::string(lightUniform_ + ".intensity").c_str()), intensity);
    }

    void Light::ApplyForAllShaders() const {
        for (GLuint shader : game->GetRenderer().GetShaders()) {
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
        lightUniform_ = "spotlights[" + std::to_string(POINT_LIGHTS_INDEX++) + "]";
    }
    void Spotlight::ApplyLight(GLuint shader) const {
        Light::ApplyLight(shader);
        glUniform3f(glGetUniformLocation(shader, std::string(lightUniform_ + ".pos").c_str()), parent->transform->position.x, parent->transform->position.y, parent->transform->position.z);
        glUniform1f(glGetUniformLocation(shader, std::string(lightUniform_ + ".range").c_str()), range);
        glUniform1f(glGetUniformLocation(shader, std::string(lightUniform_ + ".cutOffMin").c_str()), cutOffMin);
        glUniform1f(glGetUniformLocation(shader, std::string(lightUniform_ + ".cutOffMax").c_str()), cutOffMax);
    }
};
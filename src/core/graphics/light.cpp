#include "core/graphics/light.h"
#include "core/game.h"

using namespace Light;

namespace Light {
    Light::~Light() {
        if (isAdded)
            game->GetRenderer().RemoveLight(this);
    }

    void Light::Start() {
        if (!isAdded)
            game->GetRenderer().AddLight(this);
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
    
    void PointLight::ApplyLight(GLuint shader) const {
        std::string lightUniform = "pointLights[" + std::to_string(POINT_LIGHTS_INDEX++) + "]";
        glUniform1i(glGetUniformLocation(shader, std::string(lightUniform + ".enabled").c_str()), GL_TRUE);
        glUniform3f(glGetUniformLocation(shader, std::string(lightUniform + ".pos").c_str()), parent->transform->position.x, parent->transform->position.y, parent->transform->position.z);
        glUniform1f(glGetUniformLocation(shader, std::string(lightUniform + ".range").c_str()), range);
        glUniform3f(glGetUniformLocation(shader, std::string(lightUniform + ".color").c_str()), color.x, color.y, color.z);
        glUniform1f(glGetUniformLocation(shader, std::string(lightUniform + ".intensity").c_str()), intensity);
    }

    void DirectionalLight::ApplyLight(GLuint shader) const {
        std::string lightUniform = "directionalLights[" + std::to_string(DIRECTIONAL_LIGHTS_INDEX++) + "]";
        glUniform1i(glGetUniformLocation(shader, std::string(lightUniform + ".enabled").c_str()), GL_TRUE);
        glUniform3f(glGetUniformLocation(shader, std::string(lightUniform + ".dir").c_str()), dir.x, dir.y, dir.z);
        glUniform3f(glGetUniformLocation(shader, std::string(lightUniform + ".color").c_str()), color.x, color.y, color.z);
        glUniform1f(glGetUniformLocation(shader, std::string(lightUniform + ".intensity").c_str()), intensity);
    }

    void Spotlight::ApplyLight(GLuint shader) const {
        std::string lightUniform = "spotlights[" + std::to_string(POINT_LIGHTS_INDEX++) + "]";
        glUniform1i(glGetUniformLocation(shader, std::string(lightUniform + ".enabled").c_str()), GL_TRUE);
        glUniform3f(glGetUniformLocation(shader, std::string(lightUniform + ".pos").c_str()), parent->transform->position.x, parent->transform->position.y, parent->transform->position.z);
        glUniform1f(glGetUniformLocation(shader, std::string(lightUniform + ".range").c_str()), range);
        glUniform1f(glGetUniformLocation(shader, std::string(lightUniform + ".cutOffMin").c_str()), cutOffMin);
        glUniform1f(glGetUniformLocation(shader, std::string(lightUniform + ".cutOffMax").c_str()), cutOffMax);
        glUniform3f(glGetUniformLocation(shader, std::string(lightUniform + ".color").c_str()), color.x, color.y, color.z);
        glUniform1f(glGetUniformLocation(shader, std::string(lightUniform + ".intensity").c_str()), intensity);
    }
};
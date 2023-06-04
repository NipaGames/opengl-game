#pragma once

#include <opengl.h>
#include <string>
#include <typeinfo>
#include <variant>

#include "shaders.h"

namespace Shaders {
    enum class ShaderType {
        VERT,
        FRAG,
        GEOM,
        VERT_FRAG,
        VERT_FRAG_GEOM
    };
    GLuint GetShaderProgram(ShaderID);
    GLuint GetShaderProgram(const std::string&);
};

class Shader {
private:
    std::variant<std::string, Shaders::ShaderID> id_;
    // cache shader program
    mutable GLuint _program = GL_NONE;
public:
    Shader() = default;
    Shader(Shaders::ShaderID id) : id_(id) { }
    Shader(const std::string& id) : id_(id) { }
    void Use() const;
    GLuint GetProgram() const;
    Shaders::ShaderID GetID() const;
    std::string GetIDString() const;

    // this implementation sucks ass, maybe i'll come with something better later
    // it will do well enough for now
    // nevermind, these 'if constexpr' statements are really cool although they look very cursed
    
    template<typename T>
    void SetUniform(const char* name, const T& value) const {
        GLuint location = glGetUniformLocation(GetProgram(), name);

        // yanderedev switch statement
        if constexpr(std::is_same_v<T, int> || std::is_same_v<T, bool>)
            glUniform1i(location, value);
        else if constexpr(std::is_same_v<T, float>)
            glUniform1f(location, value);
        else if constexpr(std::is_same_v<T, glm::mat2>)
            glUniformMatrix2fv(location, 1, GL_FALSE, &value[0][0]);
        else if constexpr(std::is_same_v<T, glm::mat3>)
            glUniformMatrix3fv(location, 1, GL_FALSE, &value[0][0]);
        else if constexpr(std::is_same_v<T, glm::mat4>)
            glUniformMatrix4fv(location, 1, GL_FALSE, &value[0][0]);
        else if constexpr(std::is_same_v<T, glm::vec2>)
            glUniform2f(location, ((glm::vec2) value).x, ((glm::vec2) value).y);
        else if constexpr(std::is_same_v<T, glm::vec3>)
            glUniform3f(location, ((glm::vec3) value).x, ((glm::vec3) value).y, ((glm::vec3) value).z);
        else if constexpr(std::is_same_v<T, glm::vec4>)
            glUniform4f(location, ((glm::vec4) value).x, ((glm::vec4) value).y, ((glm::vec4) value).z, ((glm::vec4) value).w);
    }
};
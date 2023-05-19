#pragma once

#include <opengl.h>
#include <string>
#include <typeinfo>

#include "shaders.h"

namespace Shaders {
typedef int ShaderID;

GLuint LoadShaders(ShaderID, const std::string&, const std::string&, const std::string& geometryPath = "");
void AttachGeometryShader(ShaderID, const std::string&);
void LoadAllShaders();
GLuint GetShaderProgram(ShaderID);
};

class Shader {
private:
    Shaders::ShaderID id_;
public:
    Shader() : id_(SHADER_LIT) { }
    Shader(Shaders::ShaderID id) : id_(id) { }
    void Use() const;
    Shaders::ShaderID GetId() const { return id_; }

    // this implementation sucks ass, maybe i'll come with something better later
    // it will do well enough for now
    // nevermind, these 'if constexpr' statements are really cool although they look very cursed
    
    template<typename T>
    void SetUniform(const std::string& name, const T& value) const {
        if constexpr(std::is_same_v<T, int> || std::is_same_v<T, bool>)
            glUniform1i(glGetUniformLocation(Shaders::GetShaderProgram(id_), name.c_str()), value); 
        if constexpr(std::is_same_v<T, float>)
            glUniform1f(glGetUniformLocation(Shaders::GetShaderProgram(id_), name.c_str()), value);
        if constexpr(std::is_same_v<T, glm::mat2>)
            glUniformMatrix2fv(glGetUniformLocation(Shaders::GetShaderProgram(id_), name.c_str()), 1, GL_FALSE, &value[0][0]);
        if constexpr(std::is_same_v<T, glm::mat3>)
            glUniformMatrix3fv(glGetUniformLocation(Shaders::GetShaderProgram(id_), name.c_str()), 1, GL_FALSE, &value[0][0]);
        if constexpr(std::is_same_v<T, glm::mat4>)
            glUniformMatrix4fv(glGetUniformLocation(Shaders::GetShaderProgram(id_), name.c_str()), 1, GL_FALSE, &value[0][0]);
        if constexpr(std::is_same_v<T, glm::vec3>)
            glUniform3f(glGetUniformLocation(Shaders::GetShaderProgram(id_), name.c_str()), ((glm::vec3) value).x, ((glm::vec3) value).y, ((glm::vec3) value).z);
        if constexpr(std::is_same_v<T, glm::vec4>)
            glUniform4f(glGetUniformLocation(Shaders::GetShaderProgram(id_), name.c_str()), ((glm::vec4) value).x, ((glm::vec4) value).y, ((glm::vec4) value).z, ((glm::vec4) value).w);
    }
};
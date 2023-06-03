#include "shader.h"

#include <fstream>
#include <sstream>
#include <unordered_map>
#include <spdlog/spdlog.h>

#include <core/game.h>
#include <core/io/serializetypes.h>
#include <core/io/paths.h>

using namespace Shaders;

std::string GetShaderInfoLog(GLuint shader) {
    int logLength;
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
	if (logLength > 0){
		char* message = new char[logLength + 1];
		glGetShaderInfoLog(shader, logLength, nullptr, &message[0]);
		std::string result = &message[0];
        delete[] message;
        return result;
	}
    return "";
}

std::string GetProgramInfoLog(GLuint program) {
    int logLength;
	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
	if (logLength > 0){
		char* message = new char[logLength + 1];
		glGetProgramInfoLog(program, logLength, nullptr, &message[0]);
		std::string result = &message[0];
        delete[] message;
        return result;
	}
    return "";
}

void LoadShaderFromFile(GLuint shader, const std::string& path) {
    std::string shaderData;
    std::ifstream shaderStream(path, std::ios::in);
    if (shaderStream.is_open()) {
        std::stringstream ss;
        ss << shaderStream.rdbuf();
        shaderData = ss.str();
        shaderStream.close();
    } else {
        spdlog::error("Cannot load shader '{}'!", path);
        return;
    }
    spdlog::info("Compiling shader '{}'!", path);
    char const* shaderDataPtr = shaderData.c_str();
    glShaderSource(shader, 1, &shaderDataPtr, nullptr);
    glCompileShader(shader);

    auto shaderMessage = GetShaderInfoLog(shader);
	if (shaderMessage != "")
		spdlog::info(shaderMessage);
}

void DetachShaders(GLuint program, GLuint vert, GLuint frag, GLuint geometry = -1) {
    glDetachShader(program, vert);
    glDeleteShader(vert);

    glDetachShader(program, frag);
    glDeleteShader(frag);

    if (geometry != -1) {
        glDetachShader(program, geometry);
        glDeleteShader(geometry);
    }
}

GLuint Shaders::LoadShaders(ShaderID id, const std::string& vertexPath, const std::string& fragmentPath, const std::string& geometryPath) {
    GLuint program = glCreateProgram();
    
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    LoadShaderFromFile(vertexShader, vertexPath);
	glAttachShader(program, vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    LoadShaderFromFile(fragmentShader, fragmentPath);
	glAttachShader(program, fragmentShader);

    GLuint geometryShader = -1;
    if (geometryPath != "") {
        geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
        LoadShaderFromFile(geometryShader, geometryPath);
        glAttachShader(program, geometryShader);
    }

    glLinkProgram(program);
    auto programMessage = GetProgramInfoLog(program);
	if (programMessage != "")
		spdlog::info(programMessage);

    DetachShaders(vertexShader, fragmentShader, geometryShader);

    game->GetRenderer().shaders[id] = program;
    return program;
}

GLuint Shaders::LoadShaders(ShaderID id, const std::string& path, const ShaderType& t) {
    std::string vertexPath = "";
    std::string fragmentPath = "";
    std::string geometryPath = "";
    switch (t) {
        case ShaderType::VERT_FRAG:
            vertexPath = path + ".vert";
            fragmentPath = path + ".frag";
            break;
        case ShaderType::VERT_FRAG_GEOM:
            vertexPath = path + ".vert";
            fragmentPath = path + ".frag";
            geometryPath = path + ".geom";
            break;
    }
    return LoadShaders(id, vertexPath, fragmentPath, geometryPath);
}

GLuint Shaders::LoadShadersFromShaderDir(ShaderID id, std::string vertexPath, std::string fragmentPath, std::string geometryPath) {
    vertexPath = Paths::Path(Paths::SHADER_DIR, vertexPath);
    fragmentPath = Paths::Path(Paths::SHADER_DIR, fragmentPath);
    if (geometryPath != "")
        geometryPath = Paths::Path(Paths::SHADER_DIR, geometryPath);
    return LoadShaders(id, vertexPath, fragmentPath, geometryPath);
}

GLuint Shaders::LoadShadersFromShaderDir(ShaderID id, std::string path, const ShaderType& t) {
    path = Paths::Path(Paths::SHADER_DIR, path);
    return LoadShaders(id, path, t);
}

void Shaders::LoadAllShaders() {
    LoadShadersFromShaderDir(ShaderID::UNLIT, "unlit", ShaderType::VERT_FRAG);
    LoadShadersFromShaderDir(ShaderID::LIT, "lit", ShaderType::VERT_FRAG);
    LoadShadersFromShaderDir(ShaderID::FRAMEBUFFER, "framebuffer", ShaderType::VERT_FRAG);
    LoadShadersFromShaderDir(ShaderID::HIGHLIGHT_NORMALS, "normals", ShaderType::VERT_FRAG_GEOM);
    LoadShadersFromShaderDir(ShaderID::UI_TEXT, "text", ShaderType::VERT_FRAG);
    LoadShadersFromShaderDir(ShaderID::UI_SHAPE, "uishape", ShaderType::VERT_FRAG);
    LoadShadersFromShaderDir(ShaderID::LINE, "line", ShaderType::VERT_FRAG_GEOM);
}

GLuint Shaders::GetShaderProgram(ShaderID id) {
    return game->GetRenderer().shaders[id];
}

void Shader::Use() const {
    glUseProgram(Shaders::GetShaderProgram(id_));
}
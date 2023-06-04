#include "shader.h"

#include <fstream>
#include <sstream>
#include <unordered_map>
#include <spdlog/spdlog.h>

#include <core/game.h>
#include <core/io/serializetypes.h>
#include <core/io/paths.h>
#include <core/io/resourcemanager.h>

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
        spdlog::error("Cannot read shader file!", path);
        return;
    }
    char const* shaderDataPtr = shaderData.c_str();
    glShaderSource(shader, 1, &shaderDataPtr, nullptr);
    glCompileShader(shader);

    auto shaderMessage = GetShaderInfoLog(shader);
	if (shaderMessage != "")
		spdlog::info(shaderMessage);
}

void Resources::ShaderManager::LoadShader(GLuint program, const std::string& path, Shaders::ShaderType t) {
    GLuint shaderType;
    std::string ext;
    switch (t) {
        case ShaderType::VERT:
            ext = ".vert";
            shaderType = GL_VERTEX_SHADER;
            break;
        case ShaderType::FRAG:
            ext = ".frag";
            shaderType = GL_FRAGMENT_SHADER;
            break;
        case ShaderType::GEOM:
            ext = ".geom";
            shaderType = GL_GEOMETRY_SHADER;
            break;
        case ShaderType::VERT_FRAG:
            LoadShader(program, path, ShaderType::VERT);
            LoadShader(program, path, ShaderType::FRAG);
            return;
        case ShaderType::VERT_FRAG_GEOM:
            LoadShader(program, path, ShaderType::VERT);
            LoadShader(program, path, ShaderType::FRAG);
            LoadShader(program, path, ShaderType::GEOM);
            return;
        default:
            return;
    }
    GLuint shader = glCreateShader(shaderType);
    std::string p = Paths::Path(path_, path + ext);
    spdlog::info("Compiling shader '" + std::fs::proximate(p, path_.parent_path()).generic_string() + "'");
    LoadShaderFromFile(shader, p);
    glAttachShader(program, shader);
    glDeleteShader(shader);
}

void Resources::ShaderManager::LoadStandardShader(Shaders::ShaderID id, const std::string& path, Shaders::ShaderType t) {
    GLuint program = glCreateProgram();
    LoadShader(program, path, t);
    glLinkProgram(program);
    auto programMessage = GetProgramInfoLog(program);
	if (programMessage != "")
		spdlog::info(programMessage);
    items_[(std::string) magic_enum::enum_name(id)] = program;
}

void Resources::ShaderManager::LoadAll() {
    LoadStandardShader(ShaderID::UNLIT, "unlit", ShaderType::VERT_FRAG);
    LoadStandardShader(ShaderID::LIT, "lit", ShaderType::VERT_FRAG);
    LoadStandardShader(ShaderID::FRAMEBUFFER, "framebuffer", ShaderType::VERT_FRAG);
    LoadStandardShader(ShaderID::HIGHLIGHT_NORMALS, "normals", ShaderType::VERT_FRAG_GEOM);
    LoadStandardShader(ShaderID::UI_TEXT, "text", ShaderType::VERT_FRAG);
    LoadStandardShader(ShaderID::UI_SHAPE, "uishape", ShaderType::VERT_FRAG);
    LoadStandardShader(ShaderID::LINE, "line", ShaderType::VERT_FRAG_GEOM);
}

GLuint& Resources::ShaderManager::Get(ShaderID shader) {
    return ResourceManager::Get((std::string) magic_enum::enum_name(shader));
}

std::optional<GLuint> Resources::ShaderManager::LoadResource(const std::fs::path&) {
    throw "not implemented yet";
}

GLuint Shaders::GetShaderProgram(ShaderID shader) {
    return game->resources.shaderManager.Get(shader);
}

GLuint Shader::GetProgram() const {
    if (_program == GL_NONE)
        _program = Shaders::GetShaderProgram(id_);
    return _program;
}

void Shader::Use() const {
    glUseProgram(GetProgram());
}
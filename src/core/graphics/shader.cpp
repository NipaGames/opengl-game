#include "core/graphics/shader.h"

#include "core/game.h"

#include <fstream>
#include <sstream>
#include <unordered_map>
#include <spdlog/spdlog.h>

using namespace Shaders;

std::string GetShaderInfoLog(GLuint shader) {
    GLint result = GL_FALSE;
    int logLength;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
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
    GLint result = GL_FALSE;
    int logLength;
    glGetProgramiv(program, GL_LINK_STATUS, &result);
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

void Shaders::LoadAllShaders() {
    LoadShaders(SHADER_UNLIT, "../res/unlit.vert", "../res/unlit.frag");
    LoadShaders(SHADER_LIT, "../res/lit.vert", "../res/lit.frag");
    LoadShaders(SHADER_FRAMEBUFFER, "../res/framebuffer.vert", "../res/framebuffer.frag");
    LoadShaders(SHADER_HIGHLIGHT_NORMALS, "../res/normals.vert", "../res/normals.frag", "../res/normals.geom");
}

GLuint Shaders::GetShaderProgram(ShaderID id) {
    return game->GetRenderer().shaders[id];
}

void Shader::Use() const {
    glUseProgram(Shaders::GetShaderProgram(id_));
}
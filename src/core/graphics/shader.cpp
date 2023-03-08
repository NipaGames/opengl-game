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

GLuint Shaders::LoadShaders(ShaderID id, const std::string& vertexPath, const std::string& fragmentPath) {
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    std::string vertexShaderData;
    std::ifstream vertexShaderStream(vertexPath, std::ios::in);
    if (vertexShaderStream.is_open()) {
        std::stringstream ss;
        ss << vertexShaderStream.rdbuf();
        vertexShaderData = ss.str();
        vertexShaderStream.close();
    } else {
        spdlog::error("Cannot load vertex shader '{}'!", vertexPath);
        return 0;
    }

    std::string fragmentShaderData;
    std::ifstream fragmentShaderStream(fragmentPath, std::ios::in);
    if (fragmentShaderStream.is_open()) {
        std::stringstream ss;
        ss << fragmentShaderStream.rdbuf();
        fragmentShaderData = ss.str();
        fragmentShaderStream.close();
    } else {
        spdlog::error("Cannot load fragment shader '{}'!", fragmentPath);
        return 0;
    }

    spdlog::info("Compiling vertex shader '{}'!", vertexPath);
    char const* vertexShaderDataPtr = vertexShaderData.c_str();
    glShaderSource(vertexShader, 1, &vertexShaderDataPtr, nullptr);
    glCompileShader(vertexShader);

    auto vertexShaderMessage = GetShaderInfoLog(vertexShader);
	if (vertexShaderMessage != "")
		spdlog::info(vertexShaderMessage);

    spdlog::info("Compiling fragment shader '{}'!", fragmentPath);
    char const* fragmentShaderDataPtr = fragmentShaderData.c_str();
    glShaderSource(fragmentShader, 1, &fragmentShaderDataPtr, nullptr);
    glCompileShader(fragmentShader);

    auto fragmentShaderMessage = GetShaderInfoLog(fragmentShader);
	if (fragmentShaderMessage != "")
		spdlog::info(fragmentShaderMessage);

    spdlog::info("Linking program");
    GLuint program = glCreateProgram();
	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);
	glLinkProgram(program);

    auto programMessage = GetProgramInfoLog(program);
	if (programMessage != "")
		spdlog::info(programMessage);

    glDetachShader(program, vertexShader);
	glDetachShader(program, fragmentShader);
	
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

    game->GetRenderer().shaders[id] = program;

    return program;
}

void Shaders::LoadAllShaders() {
    LoadShaders(SHADER_UNLIT, "../res/unlit.vert", "../res/unlit.frag");
    LoadShaders(SHADER_LIT, "../res/lit.vert", "../res/lit.frag");
    LoadShaders(SHADER_FRAMEBUFFER, "../res/framebuffer.vert", "../res/framebuffer.frag");
}

GLuint Shaders::GetShaderProgram(ShaderID id) {
    return game->GetRenderer().shaders[id];
}

void Shader::Use() const {
    glUseProgram(Shaders::GetShaderProgram(id_));
}
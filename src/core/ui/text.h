#pragma once

#include <string>
#include <optional>
#include <unordered_map>
#include <freetype/freetype.h>
#include <opengl.h>

#include "core/graphics/shader.h"
#include "core/wchar.h"

namespace UI::Text {
struct Character {
    GLuint texture;
    glm::ivec2 size;
    glm::ivec2 bearing;
    int advance;
};
struct Font {
    FT_Face fontFace;
    std::unordered_map<WCHAR, Character> charMap;
};

bool Init();
std::optional<Font> LoadFontFile(const std::string&, const glm::ivec2&);
std::optional<Font> LoadFontFile(const std::string&, int);
void RenderText(Shader&, const Font&, const std::string&, glm::vec2, float, const glm::vec3&);
};
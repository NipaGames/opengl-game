#pragma once

#include <string>
#include <optional>
#include <unordered_map>

#include <ft2build.h>
#include FT_FREETYPE_H
#include <opengl.h>

#include <core/graphics/shader.h>
#include <core/wchar.h>

namespace UI::Text {
    struct Character {
        GLuint texture;
        glm::ivec2 size;
        glm::ivec2 bearing;
        int advance;
    };
    struct Font {
        FT_Face fontFace;
        int fontHeight;
        glm::ivec2 size;
        std::unordered_map<WCHAR, Character> charMap;
    };
    enum class TextAlignment {
        LEFT,
        RIGHT,
        CENTER
    };
    
    bool Init();
    // this apparently takes a shit ton of parameters, will have to make a struct or smth
    void RenderText(const Font&, const std::string&, glm::vec2, float, float, TextAlignment = TextAlignment::LEFT, float = 5);
    glm::ivec2 GetVerticalPadding(const Font&, const std::string&);
    int GetLineWidth(const Font&, const std::string&);
    std::vector<int> GetLineWidths(const Font&, const std::string&);
    int GetTextWidth(const Font&, const std::string&);
    int GetTextHeight(const Font&, const std::string&, int = 5);
};
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
        std::unordered_map<WCHAR, Character> charMap;
    };
    
    bool Init();
    void RenderText(const Font&, const std::string&, glm::vec2, float, float, float = 5);
    glm::ivec2 GetVerticalPadding(const Font&, const std::string&);
    int GetTextWidth(const Font&, const std::string&);
    int GetTextHeight(const Font&, const std::string&, int = 5);
};
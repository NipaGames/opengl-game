#include "text.h"

#include <spdlog/spdlog.h>
#include <unordered_map>
#include <iostream>

#include <core/graphics/shape.h>

using namespace UI::Text;

FT_Library freeType;
Shape charShape;
std::unordered_map<FontID, Font> fonts;
FontID currentFontPtr = 0;


bool UI::Text::Init() {
    if (FT_Init_FreeType(&freeType)) {
        spdlog::error("FreeType init failed!");
        return false;
    }
    spdlog::info("FreeType init successful.");
    charShape.GenerateVAO();

    return true;
}

bool RenderGlyphs(Font& font) {
    FT_Face& face = font.fontFace;
    bool warn = false;
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    for (WCHAR c = 0; c < font.fontFace->num_glyphs; c++) {
        if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
            warn = true;
            continue;
        }

        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer
        );

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        
        Character character = {
            texture, 
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            face->glyph->advance.x
        };
        font.charMap.insert(std::pair<WCHAR, Character>(c, character));
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    return warn;
}

std::optional<Font> UI::Text::LoadFontFile(const std::string& path, const glm::ivec2& size) {
    Font font;
    if (FT_New_Face(freeType, path.c_str(), 0, &font.fontFace)) {
        spdlog::warn("Font '{}' not loaded properly!", path);
        return std::nullopt;
    }
    FT_Set_Pixel_Sizes(font.fontFace, size.x, size.y);
    if (RenderGlyphs(font)) {
        spdlog::warn("Some glyphs not loaded of '{}'", path);
    }
    spdlog::info("Loaded font file '{}'", path);
    return font;
}

std::optional<Font> UI::Text::LoadFontFile(const std::string& path, int size) {
    return LoadFontFile(path, glm::ivec2(0, size));
}

const Font& UI::Text::GetFont(FontID id) {
    return fonts.at(id);
}

FontID UI::Text::AssignFont(Font& font) {
    fonts[currentFontPtr] = font;
    return currentFontPtr++;
}

void UI::Text::RenderText(const Font& font, const std::string& text, glm::vec2 pos, float size, float modifier) {    
    glActiveTexture(GL_TEXTURE0);
    charShape.Bind();

    for (std::string::const_iterator it = text.begin(); it != text.end(); ++it) {
        Character c = font.charMap.at(*it);

        glm::vec2 actualPos(pos.x + c.bearing.x * size, pos.y - (c.size.y - c.bearing.y) * size);
        float w = c.size.x * size * modifier;
        float h = c.size.y * size;

        float vertices[6][4] = {
            { actualPos.x,     actualPos.y + h,   0.0f, 0.0f },            
            { actualPos.x,     actualPos.y,       0.0f, 1.0f },
            { actualPos.x + w, actualPos.y,       1.0f, 1.0f },

            { actualPos.x,     actualPos.y + h,   0.0f, 0.0f },
            { actualPos.x + w, actualPos.y,       1.0f, 1.0f },
            { actualPos.x + w, actualPos.y + h,   1.0f, 0.0f }   
        };
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); 

        glBindTexture(GL_TEXTURE_2D, c.texture);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // some bitshift magic from learnopengl.com
        // just multiplies by 64 since for some reason freetype uses 1/64 pixel as a unit
        pos.x += (c.advance >> 6) * size * modifier;
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}


int UI::Text::GetTextWidth(const Font& font, const std::string& text) {
    int width = 0;
    for (std::string::const_iterator it = text.begin(); it != text.end(); ++it) {
        Character c = font.charMap.at(*it);
        width += (c.advance >> 6);
    }
    return width;
}

glm::ivec2 UI::Text::GetVerticalPadding(const Font& font, const std::string& text) {
    int max = 0;
    int min = 0;
    for (std::string::const_iterator it = text.begin(); it != text.end(); ++it) {
        Character c = font.charMap.at(*it);
        int cMax = c.bearing.y;
        int cMin = c.bearing.y - c.size.y;
        if (cMax > max) max = cMax;
        if (cMin < min) min = cMin;
    }
    return glm::ivec2(-min, max);
}

int UI::Text::GetTextHeight(const Font& font, const std::string& text) {
    auto padding = GetVerticalPadding(font, text);
    return padding[1] + padding[0];
}
#include "core/ui/text.h"

#include <spdlog/spdlog.h>
#include <unordered_map>
#include <iostream>

#include <ft2build.h>
#include FT_FREETYPE_H

using namespace UI::Text;

FT_Library freeType;
GLuint charVao, charVbo;

bool UI::Text::Init() {
    if (FT_Init_FreeType(&freeType)) {
        spdlog::error("FreeType init failed!");
        return false;
    }
    spdlog::info("FreeType init successful.");
    
    glGenVertexArrays(1, &charVao);
    glGenBuffers(1, &charVbo);
    glBindVertexArray(charVao);
    glBindBuffer(GL_ARRAY_BUFFER, charVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

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
        spdlog::error("Font '{}' not loaded properly!", path);
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

void UI::Text::RenderText(const Font& font, const std::string& text, glm::vec2 pos, float size) {
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(charVao);

    for (std::string::const_iterator it = text.begin(); it != text.end(); it++) {
        Character c = font.charMap.at(*it);

        glm::vec2 actualPos(pos.x + c.bearing.x * size, pos.y - (c.size.y - c.bearing.y) * size);
        float w = c.size.x * size;
        float h = c.size.y * size;

        float vertices[6][4] = {
            { actualPos.x,     actualPos.y + h,   0.0f, 0.0f },            
            { actualPos.x,     actualPos.y,       0.0f, 1.0f },
            { actualPos.x + w, actualPos.y,       1.0f, 1.0f },

            { actualPos.x,     actualPos.y + h,   0.0f, 0.0f },
            { actualPos.x + w, actualPos.y,       1.0f, 1.0f },
            { actualPos.x + w, actualPos.y + h,   1.0f, 0.0f }   
        };

        glBindTexture(GL_TEXTURE_2D, c.texture);
        glBindBuffer(GL_ARRAY_BUFFER, charVbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); 
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // some bitshift magic from learnopengl.com
        // just multiplies by 64 since for some reason freetype uses 1/64 pixel as a unit
        pos.x += (c.advance >> 6) * size;
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

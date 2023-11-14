#include "text.h"

#include <spdlog/spdlog.h>
#include <unordered_map>
#include <iostream>

#include <core/graphics/shape.h>
#include <core/io/resourcemanager.h>

using namespace UI::Text;

FT_Library freeType;
Shape charShape;

bool UI::Text::Init() {
    if (FT_Init_FreeType(&freeType)) {
        spdlog::error("FreeType init failed!");
        return false;
    }
    spdlog::info("FreeType init successful.");
    charShape.numVertexAttributes = 4;
    charShape.stride = 4;
    charShape.GenerateVAO();

    return true;
}

bool RenderGlyphs(Font& font) {
    FT_Face& face = font.fontFace;
    font.fontHeight = face->height >> 6;
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

std::optional<Font> Resources::FontManager::LoadResource(const std::fs::path& path) {
    std::string pathStr = path.string();
    Font font;
    const auto& additional = GetAdditionalData();
    glm::ivec2 fontSize = fontSize_;
    if (!additional.empty()) {
        fontSize = { 0, std::get<int>(GetAdditionalData().at(0)) };
    }
    if (FT_New_Face(freeType, pathStr.c_str(), 0, &font.fontFace))
        return std::nullopt;
    FT_Set_Pixel_Sizes(font.fontFace, fontSize.x, fontSize.y);
    if (RenderGlyphs(font))
        spdlog::warn("Some glyphs not loaded!", pathStr);
    font.size = fontSize;
    return std::optional<Font>(font);
}

void Resources::FontManager::SetFontSize(const glm::ivec2& size) {
    fontSize_ = size;
}

void Resources::FontManager::SetFontSize(int size) {
    SetFontSize(glm::ivec2(fontSize_.x, size));
}

void UI::Text::RenderText(const Font& font, const std::string& text, glm::vec2 pos, float size, float modifier, TextAlignment alignment, float lineSpacing) {    
    glActiveTexture(GL_TEXTURE0);
    charShape.Bind();
    std::vector<int> lineWidths = GetLineWidths(font, text);
    int textWidth = *std::max_element(lineWidths.begin(), lineWidths.end());
    int line = 0;
    glm::vec2 startPos = pos;
    for (std::string::const_iterator it = text.begin(); it != text.end(); ++it) {
        Character c = font.charMap.at(*it);
        float fontModifier = ((float) font.size.y / BASE_FONT_SIZE);
        if (*it == '\n') {
            pos.x = startPos.x;
            pos.y -= (font.fontHeight * fontModifier * size + lineSpacing);
            ++line;
            continue;
        }

        glm::vec2 actualPos;
        actualPos.y = pos.y - (c.size.y - c.bearing.y) * size;
        switch (alignment) {
            case TextAlignment::LEFT:
                actualPos.x = pos.x + c.bearing.x * size * fontModifier;
                break;
            case TextAlignment::RIGHT:
                actualPos.x = pos.x + (c.bearing.x + textWidth - lineWidths.at(line)) * size * fontModifier;
                break;
            case TextAlignment::CENTER:
                actualPos.x = pos.x + (c.bearing.x * size + (textWidth - lineWidths.at(line)) / 2.0f) * size * fontModifier;
                break;
        }
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

int UI::Text::GetLineWidth(const Font& font, const std::string& text) {
    int width = 0;
    for (std::string::const_iterator it = text.begin(); it != text.end(); ++it) {
        Character c = font.charMap.at(*it);
        width += (c.advance >> 6);
    }
    // todo: actual fix
    width += 10;
    return (int) (width * ((float) BASE_FONT_SIZE / font.size.y));
}


std::vector<int> UI::Text::GetLineWidths(const Font& font, const std::string& text) {
    std::vector<int> widths;
    if (std::count(text.begin(), text.end(), '\n') > 0) {
        std::stringstream ss(text);
        std::string ln;
        while(std::getline(ss, ln, '\n')){
            widths.push_back(GetLineWidth(font, ln));
        }
    }
    else {
        widths.push_back(GetLineWidth(font, text));
    }
    return widths;
}


int UI::Text::GetTextWidth(const Font& font, const std::string& text) {
    std::vector<int> widths = GetLineWidths(font, text);
    return *std::max_element(widths.begin(), widths.end());
}

glm::ivec2 UI::Text::GetVerticalPadding(const Font& font, const std::string& text) {
    int max = 0;
    int min = 0;
    for (std::string::const_iterator it = text.begin(); it != text.end(); ++it) {
        if (*it == '\n') {
            min = 0;
            max = 0;
        }
        Character c = font.charMap.at(*it);
        int cMax = c.bearing.y;
        int cMin = c.bearing.y - c.size.y;
        if (cMax > max) max = cMax;
        if (cMin < min) min = cMin;
    }
    return glm::ivec2(-min, max);
}

int UI::Text::GetTextHeight(const Font& font, const std::string& text, int lineSpacing) {
    int h = 0;
    int additionalRows = (int) std::count(text.begin(), text.end(), '\n');
    glm::ivec2 padding;
    if (additionalRows > 0) {
        h += additionalRows * (font.fontHeight + lineSpacing);
        std::string lastRow = text.substr(text.find_last_of('\n'));
        padding = GetVerticalPadding(font, lastRow);
    }
    else {
        padding = GetVerticalPadding(font, text);
    }
    h += (int) ((float) (padding[1] + padding[0]) * ((float) BASE_FONT_SIZE / font.size.y));
    return h;
}
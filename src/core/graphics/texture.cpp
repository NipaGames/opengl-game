#include "texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include <core/io/resourcemanager.h>

using namespace Texture;

std::optional<TextureID> Resources::TextureManager::LoadResource(const std::fs::path& path) {
    if (!std::filesystem::exists(path)) {
        spdlog::warn("Texture does not exist!");
        return std::optional<TextureID>(TEXTURE_NONE);
    }

    auto fPath = std::filesystem::absolute(path);
    std::string absPath = fPath.string();

    TextureID texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height;
    uint8_t* data = nullptr;
    data = stbi_load(absPath.c_str(), &width, &height, nullptr, 0);

    if (data == nullptr) {
        spdlog::warn("Can't load texture!");
    }
    GLuint channels = GL_RGB;
    if (path.extension() == ".png")
        channels = GL_RGBA;
    glTexImage2D(GL_TEXTURE_2D, 0, channels, width, height, 0, channels, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);

    return std::optional<TextureID>(texture);
}
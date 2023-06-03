#include "texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#include <filesystem>
#include <spdlog/spdlog.h>
#include <unordered_map>
#include <iostream>

#include <core/io/paths.h>

using namespace Texture;

std::unordered_map<std::string, TextureID> textures_;

TextureID Texture::LoadTexture(const std::string& pathStr) {
    std::filesystem::path path = pathStr;

    if (!std::filesystem::exists(path)) {
        spdlog::warn("Texture '" + pathStr + "' does not exist!");
        return TEXTURE_NONE;
    }

    auto fPath = std::filesystem::absolute(path);
    std::string absPath = fPath.string();
    std::string fileName = fPath.filename().string();
    if (textures_.count(fileName)) {
        spdlog::warn("Overwriting texture '" + fileName + "'!");
    }

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
        spdlog::warn("Can't load texture '" + pathStr + "'!");
    }
 
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);

    spdlog::info("Loaded texture '" + pathStr + "'!");
    textures_[fileName] = texture;

    return texture;
}

TextureID Texture::GetTexture(const std::string& pathStr) {
    if (textures_.count(pathStr) == 0)
        return TEXTURE_NONE;
    return textures_.at(pathStr);
}

void Texture::LoadAllTexturesFromTextureDir() {
    for (const auto & f : std::filesystem::directory_iterator(Paths::TEXTURES_DIR))
        LoadTexture(f.path().string());
}
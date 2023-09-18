#pragma once

#include <string>
#include <vector>
#include "texture.h"

namespace Cubemap {
    Texture::TextureID LoadTextureFromDir(const std::string&, bool = false);
    Texture::TextureID LoadTextureFromFaces(const std::string&, const char* const[6], bool = false);
};
#pragma once

#include <opengl.h>
#include <string>
#include <vector>

namespace Texture {
typedef GLuint TextureID;
#define TEXTURE_NONE -1
TextureID LoadTexture(const std::string&);
TextureID GetTexture(const std::string&);
void LoadAllTexturesFromTextureDir();
};
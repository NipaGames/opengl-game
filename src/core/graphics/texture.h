#pragma once

#include <opengl.h>
#include <string>
#include <vector>

namespace Texture {
typedef GLuint TextureID;
TextureID LoadTexture(const std::string&);
};
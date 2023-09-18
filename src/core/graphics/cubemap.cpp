#include "cubemap.h"

#include <stb_image.h>
#include <spdlog/spdlog.h>
#include <core/io/paths.h>

constexpr char* const DEFAULT_FACES[6] {
    "RIGHT.png",
    "LEFT.png",
    "TOP.png",
    "BOTTOM.png",
    "FRONT.png",
    "BACK.png"
};

Texture::TextureID Cubemap::LoadTextureFromDir(const std::string& dir, bool flipHorizontally) {
    return LoadTextureFromFaces(dir, DEFAULT_FACES, flipHorizontally);
}
Texture::TextureID Cubemap::LoadTextureFromFaces(const std::string& dir, const char* const faces[6], bool flipHorizontally) {
    Texture::TextureID texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
    std::string textureDir = Paths::Path(Paths::TEXTURES_DIR, dir);

    for (int i = 0; i < 6; i++) {
        int width, height, nChannels;
        std::string path = Paths::Path(textureDir, faces[i]);
        std::string pathStr = std::fs::proximate(path, Paths::TEXTURES_DIR.parent_path()).generic_string();
        spdlog::info("Loading (cubemap) texture '{}'", pathStr);
        uint8_t* img = stbi_load(path.c_str(), &width, &height, &nChannels, 0);
        if (img) {
            // texcoords for skyboxes are kinda broken but you can fix this by flipping the image lol
            // probably because opengl tries to render the textures "to the outside of the cube",
            // but in skyboxes they should be facing the insides
            // if you know any better way than this mere hack then let me know
            if (flipHorizontally) {
                uint8_t* flippedImg = new uint8_t[width * height * nChannels];
                for (int i = 0; i < height; i++) {
                    for (int j = 0; j < width; j++) {
                        for (int c = 0; c < nChannels; c++) {
                            int newPos = width * i * nChannels + j * nChannels + c;
                            int oldPos = width * (i + 1) * nChannels - j * nChannels + c;
                            flippedImg[newPos] = img[oldPos];
                        }
                    }
                }
                stbi_image_free(img);
                img = flippedImg;
            }
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, img);
        }
        else
            spdlog::warn("Failed to load cubemap face '{}'!", pathStr);
        stbi_image_free(img);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    
    return texture;
}
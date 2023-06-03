#pragma once

#include <filesystem>

namespace Paths {
    inline std::filesystem::path RESOURCES_DIR = "../../res";

    inline std::filesystem::path SHADER_DIR = RESOURCES_DIR / "shaders";
    inline std::filesystem::path TEXTURES_DIR = RESOURCES_DIR / "textures";
    inline std::filesystem::path FONTS_DIR = RESOURCES_DIR / "fonts";
    inline std::filesystem::path STAGES_DIR = RESOURCES_DIR / "stages";
    inline std::filesystem::path OBJECTS_DIR = RESOURCES_DIR / "objects";

    inline std::filesystem::path MATERIALS_PATH = RESOURCES_DIR / "materials.json";
    inline std::filesystem::path OBJECTS_PATH = RESOURCES_DIR / "objects.json";

    inline std::string Path(std::filesystem::path p, std::string subPath) {
        return std::filesystem::path(p / subPath).string();
    }
};
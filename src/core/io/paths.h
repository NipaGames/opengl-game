#pragma once
#include "fs.h"

namespace Paths {
    inline std::fs::path RESOURCES_DIR = "../../res";
    inline std::fs::path USER_SETTINGS_DIR = "../../user";

    inline std::fs::path SHADER_DIR = RESOURCES_DIR / "shaders";
    inline std::fs::path STANDARD_SHADER_DIR = SHADER_DIR / "standard";
    inline std::fs::path TEXTURES_DIR = RESOURCES_DIR / "textures";
    inline std::fs::path FONTS_DIR = RESOURCES_DIR / "fonts";
    inline std::fs::path STAGES_DIR = RESOURCES_DIR / "stages";
    inline std::fs::path MODELS_DIR = RESOURCES_DIR / "models";

    inline std::fs::path IMPORTS_PATH = RESOURCES_DIR / "imports.cfg";
    inline std::fs::path MATERIALS_PATH = RESOURCES_DIR / "materials.json";
    inline std::fs::path OBJECTS_PATH = RESOURCES_DIR / "objects.json";

    inline std::fs::path VIDEO_SETTINGS_PATH = USER_SETTINGS_DIR / "video.cfg";

    inline std::string Path(const std::fs::path& p, const std::string& subPath) {
        return std::fs::path(p / subPath).string();
    }
    inline std::string Path(const std::string& p, const std::string& subPath) {
        return Path(std::fs::path(p), subPath);
    }
};
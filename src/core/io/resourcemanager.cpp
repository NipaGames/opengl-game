#include "resourcemanager.h"

#include "files/cfg.h"
#include "paths.h"
#include "serializablestruct.h"
#include <core/game.h>

typedef std::string ResourceName;

namespace Resources {
    TextureManager::TextureManager() : ResourceManager<Texture::TextureID>(Paths::TEXTURES_DIR, ResourceName("texture")) { }
    ShaderManager::ShaderManager() : ResourceManager<GLuint>(Paths::SHADER_DIR) { }
    FontManager::FontManager() : ResourceManager<UI::Text::Font>(Paths::FONTS_DIR, ResourceName("font")) { }
    ModelManager::ModelManager() : ResourceManager<Model>(Paths::MODELS_DIR, ResourceName("model")) { }
    StageManager::StageManager() : ResourceManager<Stage>(Paths::STAGES_DIR, ResourceName("stage")) { }
};

// this sucks
void ResourceManager::ParseVideoSettings(CFG::CFGObject* root) {
    videoSettings.CopyFromCFGObject(root);
}

void ResourceManager::RestoreDefaultVideoSettings() {
    videoSettings = Resources::VideoSettings();
}

void ResourceManager::LoadAll() {
    Serializer::CFGSerializer importsSerializer = Serializer::CFGSerializer(CFG::ImportsFile());
    importsSerializer.SerializeFile(Paths::IMPORTS_PATH.string());
    imports = importsSerializer.GetRoot();

    Serializer::CFGSerializer videoSettingsSerializer = Serializer::CFGSerializer();
    videoSettingsSerializer.SerializeFile(Paths::VIDEO_SETTINGS_PATH.string());
    if (!videoSettingsSerializer.Validate(videoSettings.CreateCFGTemplate())) {
        spdlog::error("Corrupted video settings!");
        spdlog::info("Restoring defaults...");
        RestoreDefaultVideoSettings();
    }
    else {
        ParseVideoSettings(videoSettingsSerializer.GetRoot());
    }

    textureManager.LoadAll(imports->ListImports("textures"));

    shaderManager.LoadAll(imports->ListShaderImports("shaders"));

    materialsFile.SerializeFile(Paths::MATERIALS_PATH.string());
    materialsFile.Register(GAME->GetRenderer().GetMaterials());

    objectsFile.SerializeFile(Paths::OBJECTS_PATH.string());
    modelManager.LoadAll(imports->ListImports("models"));

    fontManager.LoadAll(imports->ListImports("fonts"));

    stageManager.LoadAll(imports->ListImports("stages"));
}
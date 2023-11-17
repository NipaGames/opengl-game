#include "resourcemanager.h"

#include "files/cfg.h"
#include "paths.h"
#include "serializablestruct.h"
#include <core/game.h>

#include <fstream>

typedef std::string ResourceName;

namespace Resources {
    TextureManager::TextureManager() : ResourceManager<Texture::TextureID>(Paths::TEXTURES_DIR, ResourceName("texture")) { }
    ShaderManager::ShaderManager() : ResourceManager<GLuint>(Paths::SHADER_DIR) { }
    FontManager::FontManager() : ResourceManager<UI::Text::Font>(Paths::FONTS_DIR, ResourceName("font")) { }
    ModelManager::ModelManager() : ResourceManager<Model>(Paths::MODELS_DIR, ResourceName("model")) { }
    StageManager::StageManager() : ResourceManager<Stage>(Paths::STAGES_DIR, ResourceName("stage")) { }
};

void Resources::SaveConfig(const std::fs::path& path, const SerializableStruct& config) {
    const CFG::CFGObject* root = config.CFGSerialize();
    std::string serializedString = CFG::Dump(root);
    delete root;
    std::ofstream fstream(path.string());
    fstream.write(serializedString.c_str(), serializedString.length());
}

void ResourceManager::LoadAll() {
    Serializer::CFGSerializer importsSerializer = Serializer::CFGSerializer(CFG::ImportsFile());
    importsSerializer.SerializeFile(Paths::IMPORTS_PATH.string());
    imports = importsSerializer.GetRoot();

    if (!std::fs::is_directory(Paths::USER_SETTINGS_DIR) || !std::fs::exists(Paths::USER_SETTINGS_DIR))
        std::fs::create_directory(Paths::USER_SETTINGS_DIR);
    
    LoadConfig(Paths::VIDEO_SETTINGS_PATH, videoSettings);

    textureManager.LoadAll(imports->ListImports("textures"));

    shaderManager.LoadAll(imports->ListShaderImports("shaders"));

    materialsFile.SerializeFile(Paths::MATERIALS_PATH.string());
    materialsFile.Register(GAME->GetRenderer().GetMaterials());

    objectsFile.SerializeFile(Paths::OBJECTS_PATH.string());
    modelManager.LoadAll(imports->ListImports("models"));

    fontManager.LoadAll(imports->ListImports("fonts"));

    stageManager.LoadAll(imports->ListImports("stages"));
}
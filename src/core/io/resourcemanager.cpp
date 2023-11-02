#include "resourcemanager.h"

#include "files/cfg.h"
#include <core/game.h>

void Resources::ParseVideoSettings(CFG::CFGObject* root) {
    videoSettings.gamma = root->GetItemByName<float>("gamma")->value;
}

void Resources::LoadAll() {
    Serializer::CFGSerializer importsSerializer = Serializer::CFGSerializer(CFG::ImportsFile());
    importsSerializer.SerializeFile(Paths::IMPORTS_PATH.string());
    imports = importsSerializer.GetRoot();

    Serializer::CFGSerializer videoSettingsSerializer = Serializer::CFGSerializer(CFG::VideoSettingsFile());
    videoSettingsSerializer.SerializeFile(Paths::VIDEO_SETTINGS_PATH.string());
    ParseVideoSettings(videoSettingsSerializer.GetRoot());

    textureManager.LoadAll(imports->ListImports("textures"));
    shaderManager.LoadAll();

    materialsFile.SerializeFile(Paths::MATERIALS_PATH.string());
    materialsFile.Register(GAME->GetRenderer().GetMaterials());

    objectsFile.SerializeFile(Paths::OBJECTS_PATH.string());
    modelManager.LoadAll(imports->ListImports("models"));

    fontManager.LoadAll(imports->ListImports("fonts"));
}
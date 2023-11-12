#include "resourcemanager.h"

#include "files/cfg.h"
#include <core/game.h>

// this sucks
void ResourceManager::ParseVideoSettings(CFG::CFGObject* root) {
    videoSettings.gamma = root->GetItemByName<float>("gamma")->GetValue();
    videoSettings.contrast = root->GetItemByName<float>("contrast")->GetValue();
    videoSettings.brightness = root->GetItemByName<float>("brightness")->GetValue();
    videoSettings.saturation = root->GetItemByName<float>("saturation")->GetValue();
    videoSettings.useVsync = root->GetItemByName<int>("use_vsync")->GetValue();
    videoSettings.fullscreen = root->GetItemByName<int>("fullscreen")->GetValue();
    const CFG::CFGObject* resolutionStruct = root->GetObjectByName("resolution");
    videoSettings.resolution.x = resolutionStruct->GetItemByIndex<int>(0)->GetValue();
    videoSettings.resolution.y = resolutionStruct->GetItemByIndex<int>(1)->GetValue();
    const CFG::CFGObject* fullscreenResolutionStruct = root->GetObjectByName("resolution_fullscreen");
    videoSettings.fullscreenResolution.x = fullscreenResolutionStruct->GetItemByIndex<int>(0)->GetValue();
    videoSettings.fullscreenResolution.y = fullscreenResolutionStruct->GetItemByIndex<int>(1)->GetValue();
}

void ResourceManager::RestoreDefaultVideoSettings() {
    videoSettings.gamma = 1.0;
    videoSettings.contrast = 1.0;
    videoSettings.brightness = 1.0;
    videoSettings.saturation = 1.0;
    videoSettings.useVsync = true;
    videoSettings.fullscreen = false;
    videoSettings.resolution = { BASE_WIDTH, BASE_HEIGHT };
    videoSettings.fullscreenResolution = glm::ivec2(0);
}

void ResourceManager::LoadAll() {
    Serializer::CFGSerializer importsSerializer = Serializer::CFGSerializer(CFG::ImportsFile());
    importsSerializer.SerializeFile(Paths::IMPORTS_PATH.string());
    imports = importsSerializer.GetRoot();

    Serializer::CFGSerializer videoSettingsSerializer = Serializer::CFGSerializer(CFG::VideoSettingsFile());
    videoSettingsSerializer.SerializeFile(Paths::VIDEO_SETTINGS_PATH.string());
    if (!videoSettingsSerializer.Success()) {
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
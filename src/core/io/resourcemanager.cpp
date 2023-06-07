#include "resourcemanager.h"

void Resources::LoadAll() {
    Serializer::CFGSerializer importsSerializer = Serializer::CFGSerializer(CFG::ImportsFile());
    importsSerializer.SerializeFile(Paths::IMPORTS_PATH.string());
    importsFile = importsSerializer.GetRoot();
    textureManager.LoadAll(importsFile->GetItemValues<std::string>("textures"));
    shaderManager.LoadAll();
    fontManager.LoadAll(importsFile->GetItemValues<std::string>("fonts"));
}
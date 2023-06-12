#include "resourcemanager.h"

#include <core/game.h>

void Resources::LoadAll() {
    Serializer::CFGSerializer importsSerializer = Serializer::CFGSerializer(CFG::ImportsFile());
    importsSerializer.SerializeFile(Paths::IMPORTS_PATH.string());
    imports = importsSerializer.GetRoot();

    textureManager.LoadAll(imports->GetItemValues<std::string>("textures"));
    shaderManager.LoadAll();

    materialsFile.SerializeFile(Paths::MATERIALS_PATH.string());
    materialsFile.Register(game->GetRenderer().GetMaterials());

    modelManager.LoadAll(imports->GetItemValues<std::string>("models"));
    objectsFile.SerializeFile(Paths::OBJECTS_PATH.string());
    //objectsFile.Register();

    fontManager.LoadAll(imports->GetItemValues<std::string>("fonts"));
}
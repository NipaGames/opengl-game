#include "resourcemanager.h"

#include "files/cfg.h"
#include <core/game.h>

void Resources::LoadAll() {
    Serializer::CFGSerializer importsSerializer = Serializer::CFGSerializer(CFG::ImportsFile());
    importsSerializer.SerializeFile(Paths::IMPORTS_PATH.string());
    imports = importsSerializer.GetRoot();

    textureManager.LoadAll(imports->ListImports("textures"));
    shaderManager.LoadAll();

    materialsFile.SerializeFile(Paths::MATERIALS_PATH.string());
    materialsFile.Register(game->GetRenderer().GetMaterials());

    objectsFile.SerializeFile(Paths::OBJECTS_PATH.string());
    modelManager.LoadAll(imports->ListImports("models"));

    fontManager.LoadAll(imports->ListImports("fonts"));
}
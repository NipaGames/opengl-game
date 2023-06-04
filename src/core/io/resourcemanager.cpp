#include "resourcemanager.h"

void Resources::LoadAll() {
    textureManager.LoadAll();
    shaderManager.LoadAll();
    fontManager.LoadAll();
}
#pragma once

#include "mesh.h"

#include <unordered_map>
#include <core/entity/transform.h>

struct Object {
    std::shared_ptr<Material> defaultMaterial = nullptr;
    std::unordered_map<int, std::shared_ptr<Material>> materials;
};
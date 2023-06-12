#pragma once

#include "mesh.h"

#include <core/entity/transform.h>

struct Object {
    Transform transform;
    std::vector<std::shared_ptr<Mesh>> meshes;
};
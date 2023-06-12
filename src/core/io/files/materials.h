#pragma once

#include "../serializer.h"
#include <core/graphics/material.h>

namespace Serializer {
    class MaterialSerializer : public JSONFileSerializer, public SerializerItemInterface<std::shared_ptr<Material>> {
    using JSONFileSerializer::JSONFileSerializer;
    protected:
        void ParseJSON() override;
    };
};

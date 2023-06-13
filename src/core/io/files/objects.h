#pragma once

#include "../serializer.h"
#include <core/graphics/object.h>

namespace Serializer {
    class ObjectSerializer : public JSONFileSerializer, public SerializerItemInterface<Object> {
    protected:
        void ParseJSON() override;
    };
};

#pragma once

#include "../serializer.h"
#include <core/graphics/object.h>

namespace Serializer {
    class ObjectSerializer : public JSONFileSerializer, public SerializerItemInterface<std::shared_ptr<Object>> {
    protected:
        void ParseJSON() override;
    };
};

#pragma once

#include "../serializer.h"

namespace Serializer {
    class BlueprintSerializer : public JSONFileSerializer, public SerializerItemInterface<std::vector<IComponent*>> {
    using JSONFileSerializer::JSONFileSerializer;
    protected:
        bool ParseJSON() override;
    public:
        ~BlueprintSerializer();
    };
};

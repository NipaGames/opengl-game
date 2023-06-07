#pragma once

#include "../serializer.h"
#include <core/graphics/material.h>

namespace Serializer {
    class MaterialSerializer : public JSONFileSerializer {
    protected:
        std::unordered_map<std::string, std::shared_ptr<Material>> materials_;
        void ParseJSON() override;
    public:
        std::unordered_map<std::string, std::shared_ptr<Material>>& GetMaterials();
        void AddMaterials();
        MaterialSerializer(const std::string& p) {
            SerializeFile(p);
        }
        MaterialSerializer() = default;
    };
};

#pragma once

#include "../serializer.h"
#include "blueprints.h"
#include <core/stage.h>

namespace Serializer {
    bool DeserializeComponentFromJSON(IComponent*, const nlohmann::json&, const std::string& = "");
    class StageSerializer : public JSONFileSerializer {
    protected:
        Stage stage_;
        Serializer::BlueprintSerializer* blueprints_ = nullptr;
        bool ParseJSON() override;
        std::list<Entity> ParseEntities(const nlohmann::json& entities, int* = nullptr);
    public:
        Stage& GetStage();
        void UseBlueprints(Serializer::BlueprintSerializer* blueprints) {
            blueprints_ = blueprints;
        }
        StageSerializer(const std::string& p) {
            SerializeFile(p);
        }
        StageSerializer() = default;
    };
};

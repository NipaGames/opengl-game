#pragma once

#include "../serializer.h"
#include <core/stagemanager.h>

namespace Serializer {
    class StageSerializer : public JSONFileSerializer {
    protected:
        Stage::Stage stage_;
        bool ParseJSON() override;
    public:
        Stage::Stage& GetStage();
        StageSerializer(const std::string& p) {
            SerializeFile(p);
        }
        StageSerializer() = default;
    };
};

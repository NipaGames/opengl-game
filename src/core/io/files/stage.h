#pragma once

#include "../serializer.h"
#include <core/stagemanager.h>

namespace Serializer {
    class StageSerializer : public JSONFileSerializer {
    protected:
        Stage::Stage stage_;
        void ParseJSON() override;
    public:
        Stage::Stage& GetStage();
        StageSerializer(const std::string& p) {
            SerializeFile(p);
        }
        StageSerializer() = default;
    };
};

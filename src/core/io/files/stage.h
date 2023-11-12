#pragma once

#include "../serializer.h"
#include <core/stage.h>

namespace Serializer {
    class StageSerializer : public JSONFileSerializer {
    protected:
        Stage stage_;
        bool ParseJSON() override;
    public:
        Stage& GetStage();
        StageSerializer(const std::string& p) {
            SerializeFile(p);
        }
        StageSerializer() = default;
    };
};

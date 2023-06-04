#pragma once

#include <core/io/serializer.h>
#include <core/stagemanager.h>

namespace Serializer {
    class StageSerializer : public JSONFileSerializer {
    protected:
        Stage::Stage stage_;
        void ParseJSON() override;
    public:
        Stage::Stage& GetStage();
        StageSerializer(const std::string& p) {
            Serialize(p);
        }
        StageSerializer() = default;
    };
};

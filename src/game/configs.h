#include <core/io/serializablestruct.h>
#include <core/io/paths.h>

namespace Config {
    inline std::fs::path CONTROLS_PATH = Paths::USER_SETTINGS_DIR / "controls.cfg";
    inline std::fs::path GENERAL_PATH = Paths::USER_SETTINGS_DIR / "general.cfg";

    struct Controls : SerializableStruct {
        INHERIT_COPY(Controls);

        DATA_FIELD(float, sensitivity, 1.0f);
    };

    struct General : SerializableStruct {
        INHERIT_COPY(General);

        META_COMMENT("might be a good idea to disable this on lower-end pcs");
        META_COMMENT("(or if i want to flex with 2000+ fps)");
        DATA_FIELD(bool, blurEdges, true);
    };
};
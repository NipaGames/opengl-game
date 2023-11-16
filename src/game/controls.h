#include <core/io/serializablestruct.h>
#include <core/io/paths.h>

inline std::fs::path CONTROLS_PATH = Paths::USER_SETTINGS_DIR / "controls.cfg";

struct Controls : SerializableStruct {
    DATA_FIELD(float, sensitivity, 1.0f);
};
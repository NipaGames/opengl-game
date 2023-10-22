#include "event.h"

void Event::Trigger() {
    for (auto* call : calls_) {
        call();
    }
}

// serialization
#include <core/io/serializer.h>
#include <core/io/serializetypes.h>

JSON_SERIALIZE_TYPES([](Serializer::SerializationArgs& args, const nlohmann::json& j) {
    Event event;
    args.Return(event);
    return true;
}, Event);
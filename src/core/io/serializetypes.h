#pragma once
#include <opengl.h>
#include <magic_enum/magic_enum.hpp>

#define _UNIQUE_VAR_NAME_CONCAT_(x, y) x##y
#define _UNIQUE_VAR_NAME_CONCAT(x, y) _UNIQUE_VAR_NAME_CONCAT_(x, y)
#define _UNIQUE_VAR_NAME(x) _UNIQUE_VAR_NAME_CONCAT(x, __COUNTER__)
// Return true if successful,
// Return value with (Serializer::SerializationArgs&) args.Return(val);
#define JSON_SERIALIZE_TYPES(f, ...) inline const void* _UNIQUE_VAR_NAME(_json_serialization_init_val_) = Serializer::AddJSONSerializer<__VA_ARGS__>(f)
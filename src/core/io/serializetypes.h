#pragma once
#include <opengl.h>
#include <magic_enum/magic_enum.hpp>

// this is kinda bizarre way to define serializers but i'd rather use the macro in each file globally
// than put all of them in one megafunction or array in one file

#define _UNIQUE_VAR_NAME_CONCAT_(x, y) x##y
#define _UNIQUE_VAR_NAME_CONCAT(x, y) _UNIQUE_VAR_NAME_CONCAT_(x, y)
#define _UNIQUE_VAR_NAME(x) _UNIQUE_VAR_NAME_CONCAT(x, __COUNTER__)
// Return true if successful,
// Return value with (Serializer::SerializationArgs&) args.Return(val);
#define JSON_SERIALIZE_TYPES(f, ...) static const void* _UNIQUE_VAR_NAME(_json_serialization_init_val_) = Serializer::AddJSONSerializer<__VA_ARGS__>(f)
#define JSON_SERIALIZE_ENUM(E) static const void* _UNIQUE_VAR_NAME(_json_serialization_init_val_) = Serializer::AddJSONEnumSerializer<E>()

void RegisterDefaultSerializers();
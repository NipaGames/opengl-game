#include "serializablestruct.h"
#include <core/io/files/cfg.h>
#include <spdlog/spdlog.h>

void SerializableStruct::AddMember(const std::string& name, const SerializableStructMemberData& data) {
    members_.insert({ name, data });
}

SerializableStructMemberData EMPTY_DATA = { nullptr, nullptr };
const SerializableStructMemberData& SerializableStruct::GetMemberData(const std::string& name) {
    if (members_.count(name) == 0) {
        return EMPTY_DATA;
    }
    return members_.at(name);
}

void* SerializableStruct::GetMemberAddress(const std::string& name) {
    return GetMemberData(name).addr;
}

using namespace CFG;
void SerializableStruct::CopyFromCFGObject(const CFGObject* obj) {
    for (const ICFGField* f : obj->GetItems()) {
        const SerializableStructMemberData& data = GetMemberData(f->name);
        if (data.addr == nullptr)
            continue;
        
        auto it = std::find_if(Serializer::CFG_SERIALIZERS.begin(), Serializer::CFG_SERIALIZERS.end(), [&](const auto& s) {
            return s->HasType(data.type);
        });
        if (it == Serializer::CFG_SERIALIZERS.end()) {
            spdlog::warn("No CFG serializer found for type {}!", data.type->name());
            continue;
        }
        Serializer::SerializationArgs args(Serializer::SerializerType::ANY_POINTER);
        args.ptr = data.addr;
        bool success = (*it)->fn(args, f);
        if (!success) {
            spdlog::warn("Invalid CFG value for field '{}'! (trying to serialize as {})",
                !f->name.empty() ? f->name : "UNNAMED_FIELD",
                data.type->name()
            );
        }
    }
}

CFG::CFGStructuredFields SerializableStruct::CreateCFGTemplate() {
    CFG::CFGStructuredFields fields;
    for (const auto& [k, v] : members_) {
        if (CFG_TYPES.count(v.type) == 0) {
            spdlog::warn("Unsupported serializable type for {}!", k);
            continue;
        }
        fields.push_back(CFG::Mandatory(k, CFG_TYPES.at(v.type)));
    }
    return fields;
}
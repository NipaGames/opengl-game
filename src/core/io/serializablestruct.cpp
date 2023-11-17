#include "serializablestruct.h"
#include <core/io/files/cfg.h>
#include <spdlog/spdlog.h>

void SerializableStruct::AddMember(const SerializableStructMemberData& data) {
    members_.push_back(data);
}

void* SerializableStruct::AddNewline() {
    members_.push_back({
        "#meta" + std::to_string(_metaCounter++),
        nullptr,
        nullptr,
        MetaType::NEWLINE
    });
    return nullptr;
}

void* SerializableStruct::AddComment(const std::string& comment) {
    members_.push_back({
        "#meta" + std::to_string(_metaCounter++),
        nullptr,
        nullptr,
        MetaType::COMMENT,
        comment
    });
    return nullptr;
}

SerializableStructMemberData EMPTY_DATA = { "", nullptr, nullptr };
const SerializableStructMemberData& SerializableStruct::GetMemberData(const std::string& name) {
    auto it = std::find_if(members_.begin(), members_.end(), [&](const auto& m) { return m.name == name; });
    if (it == members_.end()) {
        return EMPTY_DATA;
    }
    return *it;
}

void* SerializableStruct::GetMemberAddress(const std::string& name) {
    return GetMemberData(name).addr;
}

using namespace CFG;
const CFGObject* SerializableStruct::CFGSerialize() const {
    CFGObject* root = new CFGObject();
    for (const auto& m : members_) {
        ICFGField* field;
        if (m.metaType != MetaType::NONE) {
            CFGField<std::string>* meta = new CFGField<std::string>();
            switch(m.metaType) {
                case MetaType::NEWLINE:
                    meta->value = "\n";
                    break;
                case MetaType::COMMENT:
                    meta->value = "# " + m.metaData + "\n";
                    break;
            }
            meta->type = CFGFieldType::RAW;
            field = meta;
        }
        else {
            if (CFG_TYPES.count(m.type) == 0) {
                spdlog::warn("Cannot serialize member '{}' ({})", m.name, m.type->name());
                continue;
            }
            const auto& types = CFG_TYPES.at(m.type);
            if (types.size() == 1) {
                field = CreateNewCFGField(types.front(), m.addr);
                field->type = types.front();
            }
            else if (types.front() == CFGFieldType::STRUCT) {
                CFGObject* structObj = new CFGObject();
                structObj->type = types.front();
                int index = 0;
                for (int i = 1; i < types.size(); i++) {
                    CFGFieldType type = types.at(i);
                    if (type == CFGFieldType::STRUCT_MEMBER_REQUIRED) {
                        continue;
                    }
                    // wahoo hardcoded serializations lessgoo
                    ICFGField* structMember;
                    if (m.type == &typeid(glm::vec2))
                        structMember = CreateNewCFGField(type, &static_cast<const glm::vec2*>(m.addr)->operator[](index++));
                    else if (m.type == &typeid(glm::ivec2))
                        structMember = CreateNewCFGField(type, &static_cast<const glm::ivec2*>(m.addr)->operator[](index++));
                    structMember->type = type;
                    structObj->AddItem(structMember);
                }
                field = structObj;
            }
            else {
                continue;
            }
        }
        field->name = m.name;
        root->AddItem(field);
    }
    return root;
}

void SerializableStruct::CFGDeserialize(const CFGObject* obj) {
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

CFG::CFGStructuredFields SerializableStruct::CreateCFGTemplate() const {
    CFG::CFGStructuredFields fields;
    for (const auto& m : members_) {
        if (m.metaType != MetaType::NONE)
            continue;
        if (CFG_TYPES.count(m.type) == 0) {
            spdlog::warn("Unsupported serializable type for {}!", m.name);
            continue;
        }
        fields.push_back(CFG::Mandatory(m.name, CFG_TYPES.at(m.type)));
    }
    return fields;
}
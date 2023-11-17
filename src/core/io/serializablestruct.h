#pragma once

#include <unordered_map>
#include <opengl.h>

// hell yeah these again
// fuck the compiler!!!
namespace CFG {
    class ICFGField;
    template<typename>
    class CFGField;
    typedef CFGField<std::vector<ICFGField*>> CFGObject;
    struct CFGStructuredField;
    typedef std::vector<CFGStructuredField> CFGStructuredFields;
};

#define DATA_FIELD(T, name, val) T name = this->MemberField(#name, &name, val);

// more whackass reflection
struct SerializableStructMemberData {
    void* addr;
    const type_info* type;
};

struct SerializableStruct {
protected:
    std::unordered_map<std::string, SerializableStructMemberData> members_;
    void AddMember(const std::string&, const SerializableStructMemberData&);
    template <typename T>
    T MemberField(const std::string& name, T* addr, const T& val) {
        AddMember(name, { addr, &typeid(T) });
        return val;
    }
public:
    const SerializableStructMemberData& GetMemberData(const std::string&);
    void* GetMemberAddress(const std::string&);
    template <typename T>
    T operator=(const SerializableStruct& copyFrom) {
        T s;
        for (const auto& [k, v] : copyFrom.GetMembers()) {
            *reinterpret_cast<char*>(s.GetMemberAddress(k)) = *reinterpret_cast<char*>(v.addr);
        }
        return s;
    }
    template <typename T>
    T* GetMemberAddress(const std::string& name) {
        return static_cast<T*>(GetAddress(name));
    }
    const CFG::CFGObject* CFGSerialize() const;
    void CFGDeserialize(const CFG::CFGObject*);
    const std::unordered_map<std::string, SerializableStructMemberData>& GetMembers() const { return members_; }
    CFG::CFGStructuredFields CreateCFGTemplate() const;
};

#define INHERIT_COPY(T) T operator=(const T& s) { return SerializableStruct::operator=<T>(s); }
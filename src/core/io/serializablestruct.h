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
    template<typename T>
    T* GetMemberAddress(const std::string& name) {
        return static_cast<T*>(GetAddress(name));
    }
    void CopyFromCFGObject(const CFG::CFGObject*);
    CFG::CFGStructuredFields CreateCFGTemplate();
};
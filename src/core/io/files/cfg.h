#pragma once

#include "../serializer.h"
#include "../resourcemanager.h"
#include <vector>
#include <initializer_list>
#include <string>
#include <unordered_map>
#include <magic_enum/magic_enum.hpp>

namespace CFG {
    enum class CFGFieldType {
        STRING,
        NUMBER,
        ARRAY,
        STRUCT,
        STRUCT_MEMBER_REQUIRED
    };
    #define CFG_ARRAY(type) CFG::CFGFieldType::ARRAY, type
    #define CFG_REQUIRE(type) CFG::CFGFieldType::STRUCT_MEMBER_REQUIRED, type
    #define CFG_STRUCT(...) CFG::CFGFieldType::STRUCT, __VA_ARGS__
    #define CFG_IMPORT_ARRAY CFG_ARRAY(CFG_STRUCT(CFG_REQUIRE(CFG::CFGFieldType::STRING), CFG::CFGFieldType::STRING))
    
    struct CFGStructuredField {
        std::string name;
        std::vector<CFGFieldType> types;
        bool required = false;
        bool isObject = false;
        std::vector<CFGStructuredField> objectParams;
        template<typename ... Field>
        CFGStructuredField(const std::string& n, bool r, const Field&... t) : name(n), required(r) {
            (types.push_back(t), ...);
        }
        template<typename ... Field>
        CFGStructuredField(const std::string& n, const Field&... t) : CFGStructuredField(n, false, t...) { }
        CFGStructuredField(const std::string& n, bool r, const std::vector<CFGStructuredField>& v) :
            isObject(true),
            name(n),
            required(r),
            objectParams(v)
        { }
        CFGStructuredField(const std::string& n, const std::vector<CFGStructuredField>& v) : CFGStructuredField(n, false, v) { }
    };

    struct ICFGField {
        std::string name;
        CFGFieldType type;
        CFGField<std::vector<ICFGField*>>* parent;
        ICFGField() = default;
        ICFGField(CFGFieldType t) : type(t) { }
        ICFGField(const std::string& n, CFGFieldType t) : name(n), type(t) { }
    };

    template<typename T>
    struct CFGField : ICFGField {
    using ICFGField::ICFGField;
        T value;
        CFGField(const T& val) : value(val) { }
        const std::vector<ICFGField*>& GetItems() const { return static_cast<const std::vector<ICFGField*>&>(value); }
        std::vector<ICFGField*>& GetItems() { return static_cast<std::vector<ICFGField*>&>(value); }
        void AddItem(ICFGField* f) {
            static_cast<std::vector<ICFGField*>&>(value).push_back(f);
            f->parent = this;
        }
        template<typename F>
        const CFGField<F>* GetItemByName(const std::string& name) const {
            auto it = std::find_if(GetItems().begin(), GetItems().end(), [&](const ICFGField* field) {
                if (field == nullptr)
                    return false;
                return name == field->name;
            });
            if (it == GetItems().end())
                return nullptr;
            return static_cast<CFGField<F>*>(*it);
        }
        template<typename F>
        const CFGField<F>* GetItemByIndex(int i) const {
            if (i >= GetItems().size())
                return nullptr;
            return static_cast<CFGField<F>*>(GetItems().at(i));
        }
        const CFGField<std::vector<ICFGField*>>* GetObjectByName(const std::string& name) const {
            return GetItemByName<std::vector<CFG::ICFGField*>>(name);
        }
        const T& GetValue() const {
            return value;
        }
        template<typename F>
        std::vector<F> Values() const {
            std::vector<F> vec;
            for (const ICFGField* item : GetItems()) {
                vec.push_back(static_cast<const CFGField<F>*>(item)->GetValue());
            }
            return vec;
        }
        template<typename F>
        std::vector<F> GetItemValues(const std::string& name) const {
            const CFGObject* obj = GetObjectByName(name);
            if (obj != nullptr)
                return obj->Values<F>();
            else
                return std::vector<F>();
        }
        std::vector<Resources::Import> ListImports(const std::string& name) const {
            const CFGObject* obj = GetObjectByName(name);
            if (obj == nullptr)
                return { };
            std::vector<Resources::Import> imports;
            const std::vector<ICFGField*>& items = obj->GetItems();
            for (const ICFGField* v : items) {
                if (v->type != CFGFieldType::STRUCT) {
                    spdlog::warn("'{}' cannot be parsed as a list of imports!", name);
                    return { };
                }
                const CFGObject* import = static_cast<const CFGObject*>(v);
                if (import == nullptr)
                    continue;
                const auto* pathField = import->GetItemByIndex<std::string>(0);
                const auto* idField = import->GetItemByIndex<std::string>(1);
                if (pathField == nullptr || idField == nullptr)
                    continue;
                imports.push_back({ pathField->GetValue(), idField->GetValue() });
            }
            return imports;
        }
    };

    typedef CFGField<std::vector<ICFGField*>> CFGObject;

    template<typename T>
    struct CFGParseTreeNode {
        std::vector<CFGParseTreeNode<T>*> children;
        CFGParseTreeNode<T>* parent = nullptr;
        T value;
        ~CFGParseTreeNode() {
            children.clear();
            if constexpr (std::is_pointer<T>::value) {
                delete value;
            }
        }
        void AddChild(CFGParseTreeNode* child) {
            children.push_back(child);
            child->parent = this;
        }
    };

    using CFGStructuredFields = std::vector<CFGStructuredField>;
    class CFGFile {
    public:
        virtual CFGStructuredFields DefineFields() const { return { }; }
    };

    class ImportsFile : public CFGFile {
        CFGStructuredFields DefineFields() const override {
            return {
                { "fonts", CFG_IMPORT_ARRAY },
                { "models", CFG_IMPORT_ARRAY },
                { "shaders", CFG_IMPORT_ARRAY },
                { "stages", CFG_IMPORT_ARRAY },
                { "textures", CFG_IMPORT_ARRAY }
            };
        }
    };
}

namespace Serializer {    
    class CFGSerializer : public IFileSerializer {
    private:
        const CFG::CFGFile* file_ = nullptr;
        CFG::CFGObject* root_ = nullptr;
    public:
        ~CFGSerializer();
        CFGSerializer(const CFG::CFGFile* f) : file_(f) { }
        CFGSerializer(const CFG::CFGFile& f) : file_(&f) { }
        CFGSerializer() = default;
        static CFG::CFGObject* ParseCFG(std::stringstream&, const CFG::CFGFile* = nullptr);
        void ParseContents(std::ifstream&) override;
        CFG::CFGObject* GetRoot() { return root_; }
        void SetCFGFile(const CFG::CFGFile* f) { file_ = f; }
    };
}
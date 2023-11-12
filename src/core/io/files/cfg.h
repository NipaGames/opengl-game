#pragma once

#include "../serializer.h"
#include "../resourcemanager.h"
#include <vector>
#include <initializer_list>
#include <string>
#include <unordered_map>
#include <magic_enum/magic_enum.hpp>

namespace CFG {
    /*
    * CFGFieldType::NUMBER is kind of weird, you could argue
    * that a FLOAT will do in any case a NUMBER would, but they
    * are actually a bit different.
    * Both of them will accept INTEGER inputs, but FLOAT will
    * cast INTEGERs into floats at the validation stage. However,
    * NUMBER will keep the original type. NUMBER's type can be checked
    * with HasType<float/int>.
    * 
    * now, what i don't know is if there's actually any practical uses
    * for NUMBER where FLOAT wouldn't work since you'll have to cast it
    * anyway when retrieving the value.
    */
    enum class CFGFieldType {
        STRING,
        NUMBER,
        INTEGER,
        FLOAT,
        ARRAY,
        STRUCT,
        STRUCT_MEMBER_REQUIRED
    };
    #define CFG_ARRAY(type) CFG::CFGFieldType::ARRAY, type
    #define CFG_REQUIRE(type) CFG::CFGFieldType::STRUCT_MEMBER_REQUIRED, type
    #define CFG_STRUCT(...) CFG::CFGFieldType::STRUCT, __VA_ARGS__
    #define CFG_IMPORT CFG_REQUIRE(CFG::CFGFieldType::STRING), CFG::CFGFieldType::STRING
    #define CFG_VEC2(type) CFG_STRUCT(CFG_REQUIRE(type), CFG_REQUIRE(type))
    
    struct CFGStructuredField {
        std::string name;
        std::vector<CFGFieldType> types;
        bool required = false;
        bool isObject = false;
        std::vector<CFGStructuredField> objectParams;
        template<typename... Field>
        CFGStructuredField(const std::string& n, bool r, const Field&... t) : name(n), required(r) {
            (types.push_back(t), ...);
        }
        template<typename... Field>
        CFGStructuredField(const std::string& n, const Field&... t) : CFGStructuredField(n, false, t...) { }
        CFGStructuredField(const std::string& n, bool r, const std::vector<CFGStructuredField>& v) :
            isObject(true),
            name(n),
            required(r),
            objectParams(v)
        { }
        CFGStructuredField(const std::string& n, const std::vector<CFGStructuredField>& v) : CFGStructuredField(n, false, v) { }
    };

    inline CFGStructuredField Mandatory(const std::string& n, const std::vector<CFGStructuredField>& v) {
        return CFGStructuredField {n, true, v};
    }
    template<typename... Field>
    inline CFGStructuredField Mandatory(const std::string& n, const Field&... t) {
        return CFGStructuredField {n, true, t...};
    }

    inline CFGStructuredField Optional(const std::string& n, const std::vector<CFGStructuredField>& v) {
        return CFGStructuredField {n, false, v};
    }
    template<typename... Field>
    inline CFGStructuredField Optional(const std::string& n, const Field&... t) {
        return CFGStructuredField {n, false, t...};
    }

    class ICFGField {
    public:
        std::string name;
        CFGFieldType type;
        CFGField<std::vector<ICFGField*>>* parent;
        bool automaticallyCreated = false;
        ICFGField() = default;
        ICFGField(CFGFieldType t) : type(t) { }
        ICFGField(const std::string& n, CFGFieldType t) : name(n), type(t) { }
        virtual bool HasType(const std::type_info&) const { return false; }
        template<typename Type>
        bool HasType() const {
            return HasType(typeid(Type));
        }
    };

    template<typename T>
    class CFGField : public ICFGField {
    public:
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
        const ICFGField* GetItemByIndex(int i) const {
            if (i >= GetItems().size())
                return nullptr;
            return GetItems().at(i);
        }
        template<typename F>
        const CFGField<F>* GetItemByIndex(int i) const {
            return static_cast<const CFGField<F>*>(GetItemByIndex(i));
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
                Resources::Import importStruct;
                importStruct.id = idField->GetValue();
                importStruct.path = pathField->GetValue();
                for (int i = 2; i < import->GetItems().size(); i++) {
                    const ICFGField* additional = import->GetItemByIndex(i);
                    if (additional->automaticallyCreated)
                        continue;
                    switch(additional->type) {
                        case CFGFieldType::STRING:
                            importStruct.additionalData.push_back(import->GetItemByIndex<std::string>(i)->GetValue());
                            break;
                        case CFGFieldType::NUMBER:
                            if (import->GetItemByIndex(i)->HasType<int>())
                                importStruct.additionalData.push_back(import->GetItemByIndex<int>(i)->GetValue());
                            else
                                importStruct.additionalData.push_back(import->GetItemByIndex<float>(i)->GetValue());
                            break;
                        case CFGFieldType::INTEGER:
                            importStruct.additionalData.push_back(import->GetItemByIndex<int>(i)->GetValue());
                            break;
                        case CFGFieldType::FLOAT:
                            importStruct.additionalData.push_back(import->GetItemByIndex<float>(i)->GetValue());
                            break;
                    }
                }
                imports.push_back(importStruct);
            }
            return imports;
        }
        std::vector<Resources::ShaderImport> ListShaderImports(const std::string& name) const {
            const CFGObject* obj = GetObjectByName(name);
            if (obj == nullptr)
                return { };
            std::vector<Resources::ShaderImport> imports;
            const std::vector<ICFGField*>& items = obj->GetItems();
            for (const ICFGField* v : items) {
                if (v->type != CFGFieldType::STRUCT) {
                    spdlog::warn("'{}' cannot be parsed as a list of shader imports!", name);
                    return { };
                }
                const CFGObject* import = static_cast<const CFGObject*>(v);
                if (import == nullptr)
                    continue;
                
                const auto* idField = import->GetItemByIndex<std::string>(0);
                const auto* vertField = import->GetItemByIndex<std::string>(1);
                const auto* fragField = import->GetItemByIndex<std::string>(2);
                const auto* geomField = import->GetItemByIndex<std::string>(3);
                if (idField == nullptr || vertField == nullptr || fragField == nullptr || geomField == nullptr)
                    continue;
                Resources::ShaderImport importStruct;
                importStruct.id = idField->GetValue();
                importStruct.vertexPath = vertField->GetValue();
                importStruct.fragmentPath = fragField->GetValue();
                importStruct.geometryPath = geomField->GetValue();

                imports.push_back(importStruct);
            }
            return imports;
        }
        bool HasType(const std::type_info& type) const override {
            return typeid(T).hash_code() == type.hash_code();
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
                Optional("fonts", CFG_ARRAY(CFG_STRUCT(CFG_IMPORT, CFGFieldType::INTEGER))),
                Optional("models", CFG_ARRAY(CFG_STRUCT(CFG_IMPORT))),
                Optional("shaders", CFG_ARRAY(CFG_STRUCT(CFG_REQUIRE(CFGFieldType::STRING), CFG_REQUIRE(CFGFieldType::STRING), CFGFieldType::STRING, CFGFieldType::STRING))),
                Optional("stages", CFG_ARRAY(CFGFieldType::STRING)),
                Optional("textures", CFG_ARRAY(CFG_STRUCT(CFG_IMPORT)))
            };
        }
    };

    class VideoSettingsFile : public CFGFile {
        CFGStructuredFields DefineFields() const override {
            return {
                Mandatory("gamma", CFGFieldType::FLOAT),
                Mandatory("contrast", CFGFieldType::FLOAT),
                Mandatory("brightness", CFGFieldType::FLOAT),
                Mandatory("saturation", CFGFieldType::FLOAT),
                Mandatory("use_vsync", CFGFieldType::INTEGER),
                Mandatory("fullscreen", CFGFieldType::INTEGER),
                Mandatory("resolution", CFG_VEC2(CFGFieldType::INTEGER))
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
        bool ParseContents(std::ifstream&) override;
        CFG::CFGObject* GetRoot() { return root_; }
        void SetCFGFile(const CFG::CFGFile* f) { file_ = f; }
    };
}
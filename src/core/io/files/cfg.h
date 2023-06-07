#pragma once

#include "../serializer.h"
#include <vector>
#include <initializer_list>
#include <string>
#include <unordered_map>

namespace CFG {
    enum class CFGFieldType {
        STRING,
        NUMBER,
        ARRAY
    };
    #define CFG_STRING_ARRAY CFG::CFGFieldType::ARRAY, CFG::CFGFieldType::STRING
    #define CFG_NUM_ARRAY CFG::CFGFieldType::ARRAY, CFG::CFGFieldType::NUMBER

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
    };

    template<typename T>
    struct CFGField : ICFGField {
        T value;
        const std::vector<ICFGField*>& GetItems() const { return static_cast<const std::vector<ICFGField*>&>(value); }
        void AddItem(ICFGField* f) { static_cast<std::vector<ICFGField*>&>(value).push_back(f); }
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
        const CFGField<std::vector<ICFGField*>>* GetObjectByName(const std::string& name) const {
            return GetItemByName<std::vector<CFG::ICFGField*>>(name);
        }
        template<typename F>
        std::vector<F> Values() const {
            std::vector<F> vec;
            for (const ICFGField* item : GetItems()) {
                vec.push_back(static_cast<const CFGField<F>*>(item)->value);
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
    };

    typedef CFGField<std::vector<ICFGField*>> CFGObject;

    template<typename T>
    struct CFGParseTreeNode {
        std::vector<CFGParseTreeNode<T>*> children;
        CFGParseTreeNode<T>* parent;
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
                { "fonts", CFG_STRING_ARRAY },
                { "objects", CFG_STRING_ARRAY },
                { "shaders", CFG_STRING_ARRAY },
                { "stages", CFG_STRING_ARRAY },
                { "textures", CFG_STRING_ARRAY }
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
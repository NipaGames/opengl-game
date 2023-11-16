#include "cfg.h"

#include <sstream>
#include <stack>
#include <regex>
#include <queue>
#include <sstream>
#include <magic_enum/magic_enum.hpp>

using namespace Serializer;
using namespace CFG;

CFGParseTreeNode<std::string>* CreateIndentationTree(std::stringstream& buffer) {
    std::string line;
    int currentIndent = 0;
    CFGParseTreeNode<std::string>* root = new CFGParseTreeNode<std::string>();
    CFGParseTreeNode<std::string>* n = root;
    while (std::getline(buffer, line, '\n')) {
        std::string expr = line;
        int indent = 0;
        while (!expr.empty() && (expr.at(0) == ' ' || expr.at(0) == '\t')) {
            expr = expr.substr(1);
            indent++;
        }
        // remove trailing whitespace
        while (!expr.empty() && (expr.at(expr.length() - 1) == ' ' || expr.at(expr.length() - 1) == '\t')) {
            expr.pop_back();
        }
        if (expr.empty())
            continue;
        if (expr.at(0) == '#')
            continue;
        if (indent < currentIndent) {
            currentIndent = indent;
            n = n->parent;
        }
        else if (indent > currentIndent) {
            currentIndent = indent;
            n = n->children.at(n->children.size() - 1);
        }
        CFGParseTreeNode<std::string>* newN = new CFGParseTreeNode<std::string>();
        newN->value = expr;
        n->AddChild(newN);
    }
    return root;
}

const std::string MATCH_NUMBER = "(-?\\d*\\.?\\d+)";
const std::string MATCH_STRING = "([a-zA-Z_]\\w*|\"(.*?)\")";

ICFGField* ParseField(const std::string& val) {
    std::smatch groups;
    // number
    if (std::regex_search(val, std::regex("^" + MATCH_NUMBER + "$"))) {
        ICFGField* thisNode = nullptr;
        try {
            if (val.find('.') == std::string::npos) {
                CFGField<int>* intNode = new CFGField<int>(CFGFieldType::INTEGER);
                intNode->value = std::stoi(val);
                thisNode = intNode;
            }
            else {
                CFGField<float>* floatNode = new CFGField<float>(CFGFieldType::FLOAT);
                floatNode->value = std::stof(val);
                thisNode = floatNode;
            }
        }
        catch (std::exception){
            return nullptr;
        }
        return thisNode;
    }
    // string
    if (std::regex_search(val, groups, std::regex("^" + MATCH_STRING + "$"))) {
        CFGField<std::string>* thisNode = new CFGField<std::string>(CFGFieldType::STRING);
        std::string strVal = groups[2].matched ? groups[2] : groups[1];
        thisNode->value = strVal;
        return thisNode;
    }
    return nullptr;
}

ICFGField* ParseIndentTreeNodes(CFGParseTreeNode<std::string>* node, bool isRoot = false) {
    std::string name = "";
    std::string val = node->value;

    std::smatch groups;

    // object or array
    if (isRoot || std::regex_search(node->value, groups, std::regex("^" + MATCH_STRING + "\\s*:$"))) {
        if (!isRoot)
            name = groups[2].matched ? groups[2] : groups[1];
        CFGObject* thisNode = new CFGObject{ name, CFGFieldType::ARRAY };
        for (auto* child : node->children) {
            ICFGField* item = ParseIndentTreeNodes(child);
            if (item != nullptr)
                thisNode->AddItem(item);
            else {
                delete thisNode;
                return nullptr;
            }
        }
        return thisNode;
    }
    // field name
    if (std::regex_match(node->value, groups, std::regex("^" + MATCH_STRING + "\\s*:\\s*([^\\s].*)$"))) {
        name = groups[2].matched ? groups[2] : groups[1];
        val = groups[3];
    }
    // struct elements
    auto searchStart = val.cbegin();
    std::vector<std::smatch> vals;
    while (std::regex_search(searchStart, val.cend(), groups, std::regex("(" + MATCH_STRING + "|" + MATCH_NUMBER + ")"))) {
        vals.push_back(groups);
        // basically a hack around the fact that c++ regex provides no lookarounds
        // check that the next char is space or end
        if (groups.suffix().first != val.cend() && !std::isspace(*groups.suffix().first))
            return nullptr;
        searchStart = groups.suffix().first;
    }
    if (vals.empty())
        return nullptr;
    
    if (vals.size() > 1) {
        CFGObject* thisNode = new CFGObject{ name, CFGFieldType::STRUCT };
        for (const auto& v : vals) {
            ICFGField* field = ParseField(v[0]);
            if (field == nullptr)
                return nullptr;
            thisNode->AddItem(field);
        }
        return thisNode;
    }
    else {
        ICFGField* field = ParseField(val);
        if (field == nullptr)
            return nullptr;
        field->name = name;
        return field;
    }
    return nullptr;
}

// ONLY COPIES THE VALUE, DOES NOT RETURN ANY TYPE OR NAME!
template<typename T>
ICFGField* CreateNewCFGObject(const ICFGField* copyFrom = nullptr) {
    CFGField<T>* field = new CFGField<T>();
    if (copyFrom != nullptr)
        field->value = static_cast<const CFGField<T>*>(copyFrom)->value;
    return field;
}

ICFGField* CreateNewCFGObject(CFGFieldType type, const ICFGField* copyFrom = nullptr) {
    ICFGField* field;
    switch (type) {
        case CFGFieldType::STRING:
            field = CreateNewCFGObject<std::string>(copyFrom);
            break;
        case CFGFieldType::NUMBER:
        case CFGFieldType::FLOAT:
            field = CreateNewCFGObject<float>(copyFrom);
            break;
        case CFGFieldType::INTEGER:
            field = CreateNewCFGObject<int>(copyFrom);
            break;
        case CFGFieldType::ARRAY:
            field = CreateNewCFGObject<std::vector<ICFGField*>>(copyFrom);
            break;
        case CFGFieldType::STRUCT:
            field = CreateNewCFGObject<std::vector<ICFGField*>>(copyFrom);
            break;
    }
    field->type = type;
    return field;
}

ICFGField* CreateNewCFGObject(const ICFGField* copyFrom) {
    return CreateNewCFGObject(copyFrom->type, copyFrom);
}

bool ValidateCFGFieldType(ICFGField*, std::vector<CFGFieldType>);

bool CFG::IsValidType(CFGFieldType in, CFGFieldType expected, bool allowCasts) {
    if (in == expected)
        return true;
    if (!allowCasts)
        return false;
    switch (expected) {
        case CFGFieldType::NUMBER:
            return in == CFGFieldType::INTEGER || in == CFGFieldType::FLOAT;
        case CFGFieldType::FLOAT:
            return in == CFGFieldType::INTEGER || in == CFGFieldType::NUMBER;
    }
    return false;
}

bool ValidateSubItems(ICFGField* node, const std::vector<CFGFieldType>& types) {
    const CFGObject* arrayObject = static_cast<const CFGObject*>(node);
    // yeah could just as well be a raw loop but will keep this for now
    auto validate = [&](CFG::ICFGField* f) {
        return !ValidateCFGFieldType(f, { types.begin() + 1, types.end() });
    };
    return !std::any_of(arrayObject->GetItems().begin(), arrayObject->GetItems().end(), validate);
}

CFGField<float>* CastIntegerToFloat(const CFGField<int>* intField) {
    CFGField<float>* floatField = dynamic_cast<CFGField<float>*>(CreateNewCFGObject<float>(intField));
    floatField->value = (float) intField->value;
    floatField->type = CFGFieldType::FLOAT;
    floatField->name = intField->name;
    floatField->parent = intField->parent;
    if (intField->parent != nullptr) {
        auto& otherItems = const_cast<CFGField<int>*>(intField)->parent->GetItems();
        auto it = std::find(otherItems.begin(), otherItems.end(), intField);
        if (it != otherItems.end())
            *it = floatField;
    }
    delete intField;
    return floatField;
}

bool ValidateStruct(ICFGField* node, const std::vector<CFGFieldType>& types) {
    CFGObject* structNode;
    if (node->type != CFGFieldType::STRUCT) {
        std::string name = node->name;
        CFGFieldType type = node->type;
        ICFGField* field = CreateNewCFGObject(node);
        if (field == nullptr)
            return false;
        structNode = new CFGObject{ name, CFGFieldType::STRUCT };
        field->type = type;
        structNode->AddItem(field);
        auto& parentItems = node->parent->GetItems();
        *std::find(parentItems.begin(), parentItems.end(), node) = structNode;
        delete node;
    }
    else
        structNode = static_cast<CFGObject*>(node);
    std::queue<CFGFieldType> typesQueue(std::deque<CFGFieldType>(types.begin(), types.end()));
    std::queue<CFGFieldType> receivedTypes;
    for (const auto& item : structNode->GetItems())
        receivedTypes.push(item->type);
    int memberIndex = 0;
    while (!typesQueue.empty()) {
        if (typesQueue.front() == CFGFieldType::STRUCT_MEMBER_REQUIRED) {
            typesQueue.pop();
            if (receivedTypes.empty())
                return false;
        }
        else {
            if (receivedTypes.empty()) {
                ICFGField* field = CreateNewCFGObject(typesQueue.front());
                field->automaticallyCreated = true;
                structNode->AddItem(field);
            }
        }
        if (!receivedTypes.empty()) {
            if (!IsValidType(receivedTypes.front(), typesQueue.front()))
                return false;
            if ((receivedTypes.front() == CFGFieldType::INTEGER && typesQueue.front() == CFGFieldType::FLOAT)) {
                // cast integer into float
                structNode->GetItems()[memberIndex] = CastIntegerToFloat(structNode->GetItemByIndex<int>(memberIndex));
            }
        }
        typesQueue.pop();
        if (!receivedTypes.empty()) {
            receivedTypes.pop();
            ++memberIndex;
        }
    }
    if (!receivedTypes.empty())
        return false;
    return true;
}

bool ValidateCFGFieldType(ICFGField* node, std::vector<CFGFieldType> types) {
    if (types.empty())
        return false;
    if (node == nullptr)
        return false;
    if (types.at(0) == CFGFieldType::STRUCT) {
        bool success = ValidateStruct(node, { types.begin() + 1, types.end() });
        if (!success) {
            std::stringstream expected;
            for (auto it = types.cbegin() + 1; it != types.cend(); it++) {
                bool req = false;
                if (*it == CFGFieldType::STRUCT_MEMBER_REQUIRED) {
                    req = true;
                    ++it;
                }
                expected << fmt::format("{{{}}}{}", req ? "req" : "opt", magic_enum::enum_name(*it));
                if ((it + 1) != types.cend())
                    expected << ", ";
            }
            std::stringstream received;
            if (node->type != CFGFieldType::STRUCT)
                received << magic_enum::enum_name(node->type);
            else {
                const CFGObject* structNode = static_cast<const CFGObject*>(node);
                for (auto it = structNode->GetItems().cbegin(); it != structNode->GetItems().cend(); it++) {
                    received << magic_enum::enum_name((*it)->type);
                    if ((it + 1) != structNode->GetItems().cend())
                        received << ", ";
                }
            }
            spdlog::warn("Invalid struct field types! (Expected [{}] but received [{}])", expected.str(), received.str());
        }
        return success;
    }
    if (!IsValidType(node->type, types.at(0))) {
        spdlog::warn("Invalid field type for '{}'! (Expected {} but received {})",
            !node->name.empty() ? node->name : "UNNAMED_FIELD",
            magic_enum::enum_name(types.at(0)),
            magic_enum::enum_name(node->type));
        return false;
    }
    if ((node->type == CFGFieldType::INTEGER && types.at(0) == CFGFieldType::FLOAT)) {
        node = CastIntegerToFloat(dynamic_cast<const CFGField<int>*>(node));
    }
    if (types.at(0) == CFGFieldType::ARRAY) {
        if (!ValidateSubItems(node, types))
            return false;
    }
    return true;
}

bool ValidateCFGFields(CFGObject* node, const CFGStructuredFields& fields) {
    for (const auto& f : fields) {
        auto it = std::find_if(node->GetItems().begin(), node->GetItems().end(), [&](const ICFGField* field) { return f.name == field->name; });
        if (it == node->GetItems().end()) {
            if (f.required) {
                spdlog::warn("Missing mandatory field '{}'!", f.name);
                return false;
            }
            else {
                ICFGField* newField;
                if (f.isObject)
                    newField = new CFGObject();
                else
                    newField = CreateNewCFGObject(f.types.at(0));
                newField->automaticallyCreated = true;
                newField->name = f.name;
                node->AddItem(newField);
                continue;
            }
        }
        if (f.isObject) {
            CFGObject* object = static_cast<CFGObject*>(*it);
            if (object == nullptr)
                return false;
            if(!ValidateCFGFields(object, f.objectParams))
                return false;
        }
        else {
            if(!ValidateCFGFieldType(*it, f.types))
                return false;
        }
    }
    return true;
}

bool ValidateCFG(CFGObject* root, const CFGFile* fileFormat) {
    return ValidateCFGFields(root, fileFormat->DefineFields());
}

CFGSerializer::~CFGSerializer() {
    delete root_;
}

CFGObject* ParseIndentTree(CFGParseTreeNode<std::string>* strRoot) {
    return static_cast<CFGObject*>(ParseIndentTreeNodes(strRoot, true));
}

CFGObject* CFGSerializer::ParseCFG(std::stringstream& buffer, const CFGFile* fileFormat) {
    CFGParseTreeNode<std::string>* strRoot = CreateIndentationTree(buffer);
    CFGObject* root = ParseIndentTree(strRoot);
    delete strRoot;
    if (root == nullptr) {
        spdlog::warn("Invalid CFG format!");
        delete root;
        return nullptr;
    }
    if (fileFormat != nullptr) {
        if (!ValidateCFG(root, fileFormat)) {
            spdlog::warn("Field validation failed!");
            delete root;
            return nullptr;
        }
    }
    return root;
}

bool CFGSerializer::ParseContents(std::ifstream& f) {
    std::stringstream buffer;
    buffer << f.rdbuf();
    root_ = ParseCFG(buffer, file_);
    if (root_ == nullptr) {
        // create an empty object, don't wan't to null check every time
        root_ = new CFGObject();
        spdlog::warn("({}) CFG parsing failed!", path_);
        return false;
    }
    return true;
}

bool CFGSerializer::Validate(const CFGStructuredFields& fields) {
    return ValidateCFGFields(root_, fields);
}
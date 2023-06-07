#include "cfg.h"

#include <sstream>
#include <stack>
#include <regex>

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
            expr = expr.substr(0, expr.length() - 1);
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

ICFGField* ParseIndentTreeNodes(CFGParseTreeNode<std::string>* node, bool isRoot = false) {
    std::string name = "";
    std::string val = node->value;

    std::smatch matches;
    // object or array
    if (isRoot || std::regex_search(node->value, matches, std::regex("^(\\w+|\"(.*)\")\\s*:$"))) {
        if (!isRoot)
            name = matches[2].matched ? matches[2] : matches[1];
        CFGField<std::vector<ICFGField*>>* thisNode = new CFGField<std::vector<ICFGField*>>{ name, CFGFieldType::ARRAY };
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
    else {
        if (std::regex_search(node->value, matches, std::regex("^(\\w+|\"(.*)\")\\s*:\\s*([^\\s].*)$"))) {
            name = matches[2].matched ? matches[2] : matches[1];
            val = matches[3];
        }
    }
    // number
    if (std::regex_search(val, std::regex("^\\d*\\.?\\d+$"))) {
        CFGField<float>* thisNode = new CFGField<float>{ name, CFGFieldType::NUMBER };
        thisNode->value = std::stof(val);
        return thisNode;
    }
    // string
    else if (std::regex_search(val, matches, std::regex("^(\\w+|\"(.*)\")$"))) {
        CFGField<std::string>* thisNode = new CFGField<std::string>{ name, CFGFieldType::STRING };
        std::string val = matches[2].matched ? matches[2] : matches[1];
        thisNode->value = val;
        return thisNode;
    }
    return nullptr;
}

bool ValidateCFGFieldType(const ICFGField* node, std::vector<CFGFieldType> types) {
    if (types.empty())
        return false;
    if (node == nullptr)
        return false;
    if (node->type != types.at(0)) {
        spdlog::warn("Invalid field type for '{}'!", !node->name.empty() ? node->name : "UNNAMED_FIELD");
        return false;
    }
    if (types.at(0) == CFGFieldType::ARRAY) {
        const CFGObject* arrayObject = static_cast<const CFGObject*>(node);
        for (const ICFGField* f : arrayObject->GetItems()) {
            if (!ValidateCFGFieldType(f, { types.begin() + 1, types.end() }))
                return false;
        }
    }
    return true;
}

ICFGField* CreateNewCFGObject(std::vector<CFGFieldType> types) {
    ICFGField* field;
    switch (types.at(0)) {
        case CFGFieldType::STRING:
            field = new CFGField<std::string>();
            break;
        case CFGFieldType::NUMBER:
            field = new CFGField<float>();
            break;
        case CFGFieldType::ARRAY:
            field = new CFGObject();
            break;
    }
    return field;
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
                    newField = CreateNewCFGObject(f.types);
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
        spdlog::warn("Invalid CFG field value(s)!");
        delete root;
        return nullptr;
    }
    if (fileFormat != nullptr) {
        if (!ValidateCFG(root, fileFormat)) {
            return nullptr;
        }
    }
    return root;
}

void CFGSerializer::ParseContents(std::ifstream& f) {
    std::stringstream buffer;
    buffer << f.rdbuf();
    root_ = ParseCFG(buffer, file_);
    if (root_ == nullptr) {
        // create an empty object, don't wan't to null check every time
        root_ = new CFGObject();
        spdlog::warn("({}) Incorrect CFG file format!", path_);
    }
}
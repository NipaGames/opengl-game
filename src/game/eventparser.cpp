#include "eventparser.h"
#include "strutils.h"

#include <regex>
#include <sstream>
#include <iostream>
#include <core/stagemanager.h>

EventReturnStatus EventParser::ParseCommand(const std::string& cmd) {
    std::smatch groups;
    std::string event;
    EventArgs args;

    if (!std::regex_search(cmd, groups, std::regex("^\\s*(\\w+)\\s*\\((.*)\\)\\s*$"))) {
        return EventReturnStatus::INVALID_SYNTAX;
    }
    event = groups[1];
    std::string argsStr = groups[2].str();
    TrimWhitespace(argsStr);
    while (!argsStr.empty()) {
        // string
        if (argsStr.at(0) == '"' || argsStr.at(0) == '\'') {
            char delim = argsStr.at(0);
            argsStr.erase(0, 1);
            size_t close = argsStr.find(delim);
            if (close == std::string::npos) {
                return EventReturnStatus::INVALID_SYNTAX;
            }
            std::string str = argsStr.substr(0, close);
            args.push_back(str);
            argsStr.erase(0, close + 1);
        }
        // number
        else if (std::isdigit(argsStr.at(0))) {
            std::stringstream num;
            while (!argsStr.empty() && std::isdigit(argsStr.at(0))) {
                num << argsStr.at(0);
                argsStr.erase(0, 1);
            }
            int n;
            try {
                n = std::stoi(num.str());
            }
            catch (std::exception& e) {
                spdlog::warn(e.what());
                return EventReturnStatus::INVALID_PARAMS;
            }
            args.push_back(n);
        }
        // keyword, can't start with a numeral
        else if (std::isalpha(argsStr.at(0)) || argsStr.at(0) == '_') {
            std::stringstream keyword;
            while (!argsStr.empty() && (std::isalpha(argsStr.at(0)) || argsStr.at(0) == '_' || std::isdigit(argsStr.at(0)))) {
                keyword << argsStr.at(0);
                argsStr.erase(0, 1);
            }
            std::string keywordStr = keyword.str();
            if (!HasKeywordDefined(keywordStr)) {
                spdlog::warn("Invalid keyword '{}'!", keywordStr);
                return EventReturnStatus::INVALID_PARAMS;
            }
            args.push_back(GetKeyword(keywordStr));
        }
        else {
            return EventReturnStatus::INVALID_SYNTAX;
        }
        TrimWhitespace(argsStr);
        if (argsStr.empty())
            break;
        if (argsStr[0] != ',')
            return EventReturnStatus::INVALID_SYNTAX;
        argsStr.erase(0, 1);
        TrimWhitespace(argsStr);
        if (argsStr.empty())
            return EventReturnStatus::INVALID_SYNTAX;
    }
    return manager_->CallEvent(event, args);
}


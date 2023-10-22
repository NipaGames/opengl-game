#include "eventparser.h"

#include <regex>
#include <sstream>
#include <iostream>

void TrimWhitespace(std::string& str) {
    while (!str.empty() && str.at(0) == ' ') {
        str.erase(0, 1);
    }
}

EventReturnStatus EventParser::ParseCommand(const std::string& cmd) {
    std::smatch groups;
    std::string event;
    EventArgs args;
    if (!std::regex_search(cmd, groups, std::regex("^\\s*(\\w+)\\s*\\((.*)\\)\\s*$"))) {
        return EventReturnStatus::INVALID_SYNTAX;
    }
    event = groups[1];
    std::string argsStr = groups[2].str();
    while (!argsStr.empty()) {
        TrimWhitespace(argsStr);
        if (argsStr.empty()) {
            return EventReturnStatus::INVALID_SYNTAX;
        }
        if (argsStr.at(0) == '"') {
            argsStr.erase(0, 1);
            size_t close = argsStr.find('"');
            if (close == std::string::npos) {
                return EventReturnStatus::INVALID_SYNTAX;
            }
            std::string str = argsStr.substr(0, close);
            args.push_back(str);
            argsStr.erase(0, close + 1);
        }
        // number
        else if (std::isdigit(argsStr.at(0))) {
            std::stringstream numStr;
            while (!argsStr.empty() && std::isdigit(argsStr.at(0))) {
                numStr << argsStr.at(0);
                argsStr.erase(0, 1);
            }
            int n;
            try {
                n = std::stoi(numStr.str());
            }
            catch (std::exception& e) {
                spdlog::warn(e.what());
                return EventReturnStatus::INVALID_PARAMS;
            }
            args.push_back(n);
        }
        else {
            return EventReturnStatus::INVALID_SYNTAX;
        }
        TrimWhitespace(argsStr);
        if (argsStr.empty())
            break;
        if (argsStr[0] != ',') {
            return EventReturnStatus::INVALID_SYNTAX;
        }
        argsStr.erase(0, 1);
    }
    return manager_->CallEvent(event, args);
}
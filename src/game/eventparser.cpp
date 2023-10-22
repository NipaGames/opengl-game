#include "eventparser.h"

#include <regex>

bool EventParser::ParseCommand(const std::string& cmd) {
    std::smatch groups;
    std::string event;
    EventArgs args;
    if (!std::regex_search(cmd, groups, std::regex("^\\s*(\\w+)\\s*\\((.*)\\)\\s*$"))) {
        spdlog::warn("Invalid event format!");
        return false;
    }
    event = groups[1];
    std::string argsStr = groups[2].str();
    return manager_->CallEvent(event, args);
}
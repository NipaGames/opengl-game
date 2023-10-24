#include "console.h"
#include "strutils.h"

#include <spdlog/spdlog.h>

CommandReturnStatus Console::ExecuteCommand(const std::string& cmd, const std::string& args) {
    if (cmd == "exit")
        return CommandReturnStatus::EXIT_CONSOLE;
    if (commands_.count(cmd) == 0) {
        return CommandReturnStatus::COMMAND_NOT_FOUND;
    }
    commands_.at(cmd)(args);
    return CommandReturnStatus::DEFAULT;
}

CommandReturnStatus Console::ExecuteCommandInput(std::string line) {
    TrimWhitespace(line);
    std::string commandName = line;
    // get the name from the characters before space
    size_t spacePos = commandName.find(' ');
    if (spacePos != std::string::npos) {
        commandName.resize(spacePos);
    }
    if (commandName.empty())
        return CommandReturnStatus::DEFAULT;
    // to lowercase
    for (int i = 0; i < commandName.length(); i++) {
        commandName[i] = std::tolower(commandName.at(i));
    }

    std::string args = "";
    if (spacePos != std::string::npos && spacePos < line.length()) {
        args = line.substr(spacePos + 1);
    }
    TrimWhitespace(args);
    TrimTrailingWhitespace(args);
    return ExecuteCommand(commandName, args);
}

void Console::RegisterCommand(const std::string& name, ConsoleCommand fn) {
    commands_[name] = fn;
}

std::vector<std::string> Console::ListCommands() const {
    // unfortunately there is no standard function for retrieving the keys
    return ListKeys(commands_);
}

std::vector<std::string> Console::SplitArgs(std::string argsStr) {
    std::vector<std::string> args;
    TrimWhitespace(argsStr);
    while (!argsStr.empty()) {
        size_t spacePos = argsStr.find(' ');
        if (spacePos == std::string::npos) {
            args.push_back(argsStr);
            break;
        }
        args.push_back(argsStr.substr(0, spacePos));
        argsStr.erase(0, spacePos);
        TrimWhitespace(argsStr);
    }
    return args;
}
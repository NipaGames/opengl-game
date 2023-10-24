#pragma once

#include <iostream>
#include <functional>
#include <vector>

enum class CommandReturnStatus : int {
    DEFAULT,
    EXIT_CONSOLE,
    COMMAND_NOT_FOUND
};

using ConsoleCommand = std::function<void(const std::string&)>;
class Console {
private:
    std::unordered_map<std::string, ConsoleCommand> commands_;
public:
    CommandReturnStatus ExecuteCommand(const std::string&, const std::string&);
    CommandReturnStatus ExecuteCommandInput(std::string);
    void RegisterCommand(const std::string&, ConsoleCommand);
    std::vector<std::string> ListCommands() const;
    static std::vector<std::string> SplitArgs(std::string);
};
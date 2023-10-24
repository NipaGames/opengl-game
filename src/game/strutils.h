#pragma once

#include <string>
#include <vector>
#include <unordered_map>

void TrimWhitespace(std::string&);
void TrimTrailingWhitespace(std::string&);
// i don't really use other map types than unordered_map so not gonna generalize this
template <typename T>
std::vector<std::string> ListKeys(const std::unordered_map<std::string, T>& map) {
    std::vector<std::string> keys;
    for (const auto& [k, v] : map) {
        keys.push_back(k);
    }
    return keys;
}
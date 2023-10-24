#include "strutils.h"

void TrimWhitespace(std::string& str) {
    while (!str.empty() && std::isspace(str.at(0))) {
        str.erase(0, 1);
    }
}


void TrimTrailingWhitespace(std::string& str) {
    while (!str.empty() && std::isspace(str.at(str.length() - 1))) {
        str.pop_back();
    }
}
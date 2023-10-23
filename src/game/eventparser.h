#pragma once

#include <unordered_map>
#include "event.h"

class EventParser {
private:
    EventManager* manager_;
    std::unordered_map<std::string, EventParameter> keywords_;
public:
    EventParser(EventManager* mgr) : manager_(mgr) { }
    EventReturnStatus ParseCommand(const std::string&);
    template<typename T>
    void SetKeyword(const std::string& name, T val) {
        keywords_[name] = val;
    }
    const EventParameter& GetKeyword(const std::string& name) const {
        return keywords_.at(name);
    }
    bool HasKeywordDefined(const std::string& name) {
        return keywords_.count(name) != 0;
    }
    template <typename T>
    const T& GetKeyword(const std::string& name) const {
        return std::get<T>(GetKeyword(name));
    }
};

inline EventParser EVENT_PARSER = EventParser(&EVENT_MANAGER);
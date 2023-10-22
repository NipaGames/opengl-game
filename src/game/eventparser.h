#pragma once

#include "event.h"

class EventParser {
private:
    EventManager* manager_;
public:
    EventParser(EventManager* mgr) : manager_(mgr) { }
    EventReturnStatus ParseCommand(const std::string&);
};

inline EventParser EVENT_PARSER = EventParser(&EVENT_MANAGER);
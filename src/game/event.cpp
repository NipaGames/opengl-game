#include "event.h"
#include "eventparser.h"
#include "strutils.h"

void EventManager::RegisterEvent(const std::string& id, void(*event)(const EventArgs&)) {
    events_[id] = [=](const EventArgs& args) {
        event(args);
        return EventReturnStatus::OK;
    };
}
EventReturnStatus EventManager::CallEvent(const std::string& id, const EventArgs& args) {
    if (!events_.count(id)) {
        spdlog::warn("Event '{}' not found!", id);
        return EventReturnStatus::EVENT_NOT_FOUND;
    }
    return events_[id](args);
}
std::vector<std::string> EventManager::ListEvents() const {
    return ListKeys(events_);
}

void Event::Trigger() {
    for (const std::string& c : commands_) {
        parser_->ParseCommand(c);
    }
}

void Event::AddCommand(const std::string& command) {
    commands_.push_back(command);
}

// serialization
#include <core/io/serializer.h>
#include <core/io/serializetypes.h>

JSON_SERIALIZE_TYPES([](Serializer::SerializationArgs& args, const nlohmann::json& j) {
    Event event;
    if (!j.is_array())
        return false;
    for (const auto& cItem : j.items()) {
        const auto& c = cItem.value();
        if (!c.is_string())
            return false;
        event.AddCommand(c);
    }
    args.Return(event);
    return true;
}, Event);
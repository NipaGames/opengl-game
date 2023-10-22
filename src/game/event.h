// not sure if these should actually be part of the core library

#pragma once

#include <vector>
#include <unordered_map>
#include <variant>
#include <functional>

#include <spdlog/spdlog.h>

class Event {
private:
    std::vector<void(*)()> calls_;
public:
    void Trigger();
};

// std::any could also probably work but since i don't plan on having too many types i'll just use std::variant
using EventParameter = std::variant<int, std::string>;
using EventArgs = std::vector<EventParameter>;
using EventFn = std::function<bool(const EventArgs&)>;

class EventManager {
private:
    std::unordered_map<std::string, EventFn> events_;

    template <size_t N, typename... Types>
    using _nthT = std::tuple_element_t<N, std::tuple<Types...>>;
public:
    // this is fucked up
    template <typename... Args>
    void RegisterEvent(const std::string& id, void(*va_event)(Args...)) {
        EventFn event = [=](const EventArgs& args) {
            if (sizeof...(Args) != args.size()) {
                spdlog::warn("Wrong number of parameters given when calling '{}'!", id);
                return false;
            }
            int i = 0;
            bool correctTypes = true;
            // wahoo a lambda inside a lambda
            ([&] {
                if (!std::holds_alternative<Args>(args.at(i))) {
                    spdlog::warn("Type at index {} doesn't match when calling '{}'!", i, id);
                    correctTypes = false;
                }
                ++i;
            } (), ...);
            if (!correctTypes)
                return false;
            
            // so basically we need to unpack the event args and pass each element as a parameter
            // afaik each case with different param length needs to be hardcoded, at the moment we have these up to 3 params
            // if you know any other possible approach then please tell me because this is dumb and ugly as hell
            // switch doesn't work here because we need these constexprs

            if (args.size() == 0) {
                ((void(*)()) va_event)();
                return true;
            }

            if constexpr (sizeof...(Args) >= 1) {
                if (args.size() == 1) {
                    ((void(*)(_nthT<0, Args...>)) va_event)(std::get<_nthT<0, Args...>>(args.at(0)));
                    return true;
                }
            }

            if constexpr (sizeof...(Args) >= 2) {
                if (args.size() == 2) {
                    ((void(*)(_nthT<0, Args...>, _nthT<1, Args...>)) va_event)(std::get<_nthT<0, Args...>>(args.at(0)), std::get<_nthT<1, Args...>>(args.at(1)));
                    return true;
                }
            }
            
            if constexpr (sizeof...(Args) >= 3) {
                if (args.size() == 3) {
                    ((void(*)(_nthT<0, Args...>, _nthT<1, Args...>, _nthT<2, Args...>)) va_event)(std::get<_nthT<0, Args...>>(args.at(0)), std::get<_nthT<1, Args...>>(args.at(1)), std::get<_nthT<2, Args...>>(args.at(2)));
                    return true;
                }
            }

            return false;
        };
        events_[id] = event;
    }
    void RegisterEvent(const std::string& id, void(*event)(const EventArgs&)) {
        events_[id] = [=](const EventArgs& args) {
            event(args);
            return true;
        };
    }
    bool CallEvent(const std::string& id, const EventArgs& args) {
        if (!events_.count(id)) {
            spdlog::warn("Event '{}' not found!", id);
            return false;
        }
        return events_[id](args);
    }
    template <typename... Args>
    bool CallEvent(const std::string& id, Args... va_args) {
        EventArgs args;
        ([&] {
            args.push_back(va_args);
        } (), ...);
        return CallEvent(id, args);
    }
};
// yeah fuck singletons whatever
// this class approach could be handy if we wan't to have multiple event pools in future
inline EventManager EVENT_MANAGER;

#define REGISTER_EVENT_(event, mgr) mgr.RegisterEvent(#event, event)
#define REGISTER_EVENT(event) REGISTER_EVENT_(event, EVENT_MANAGER)
// might as well add a call macro too
#define CALL_EVENT_(event, mgr, ...) mgr.CallEvent(event, __VA_ARGS__)
#define CALL_EVENT(event, ...) CALL_EVENT_(event, EVENT_MANAGER, __VA_ARGS__)

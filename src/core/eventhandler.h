#pragma once

#include <map>
#include <functional>

template <typename E>
class EventHandler {
private:
    std::multimap<E, std::function<void()>> events_;
public:
    void Dispatch(E e) {
        for (auto event : events_) {
            if (event.first == e)
                event.second();
        }
    }
    void Subscribe(E e, std::function<void()> fn) {
        events_.insert({ e, fn });
    }
    void ClearEvents() {
        events_.clear();
    }
};
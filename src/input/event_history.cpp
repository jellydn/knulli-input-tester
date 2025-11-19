#include "event_history.h"

namespace knulli {
namespace input {

EventHistory::EventHistory() {
}

EventHistory::~EventHistory() {
    clear();
}

void EventHistory::addEvent(const InputEvent& event) {
    std::unique_lock lock(mutex);
    
    events.push_back(event);
    
    // Circular buffer: remove oldest if exceeded max
    while (events.size() > MAX_EVENTS) {
        events.pop_front();
    }
}

std::vector<InputEvent> EventHistory::getEvents(size_t count) const {
    std::shared_lock lock(mutex);
    
    std::vector<InputEvent> result;
    size_t start = (events.size() > count) ? events.size() - count : 0;
    
    for (size_t i = start; i < events.size(); ++i) {
        result.push_back(events[i]);
    }
    
    return result;
}

std::vector<InputEvent> EventHistory::getEventsForDevice(int deviceId) const {
    std::shared_lock lock(mutex);
    
    std::vector<InputEvent> result;
    for (const auto& event : events) {
        if (event.deviceId == deviceId) {
            result.push_back(event);
        }
    }
    
    return result;
}

std::vector<InputEvent> EventHistory::getEventsInTimeRange(
    std::chrono::milliseconds start,
    std::chrono::milliseconds end) const {
    
    std::shared_lock lock(mutex);
    std::vector<InputEvent> result;
    
    // Not implemented - requires timestamp comparison
    return result;
}

void EventHistory::clear() {
    std::unique_lock lock(mutex);
    events.clear();
}

size_t EventHistory::size() const {
    std::shared_lock lock(mutex);
    return events.size();
}

bool EventHistory::isEmpty() const {
    std::shared_lock lock(mutex);
    return events.empty();
}

const InputEvent* EventHistory::getEventAt(size_t index) const {
    std::shared_lock lock(mutex);
    
    if (index < events.size()) {
        return &events[index];
    }
    return nullptr;
}

const InputEvent* EventHistory::getLatestEvent() const {
    std::shared_lock lock(mutex);
    
    if (!events.empty()) {
        return &events.back();
    }
    return nullptr;
}

} // namespace input
} // namespace knulli

#include "event_filter.h"

namespace knulli {
namespace input {

EventFilter::EventFilter() {
}

EventFilter::~EventFilter() {
    clearFilters();
}

void EventFilter::filterByDeviceId(int deviceId) {
    filters.push_back([deviceId](const InputEvent& event) {
        return event.deviceId == deviceId;
    });
}

void EventFilter::filterByEventType(InputType type) {
    filters.push_back([type](const InputEvent& event) {
        return event.type == type;
    });
}

void EventFilter::filterByEventCode(InputCode code) {
    filters.push_back([code](const InputEvent& event) {
        return event.code == code;
    });
}

void EventFilter::filterByDeviceIdRange(int minId, int maxId) {
    filters.push_back([minId, maxId](const InputEvent& event) {
        return event.deviceId >= minId && event.deviceId <= maxId;
    });
}

void EventFilter::setCustomFilter(std::function<bool(const InputEvent&)> predicate) {
    filters.push_back(predicate);
}

std::vector<InputEvent> EventFilter::apply(const EventHistory& history) const {
    std::vector<InputEvent> result = history.getEvents();
    
    // Apply all filters
    for (const auto& filter : filters) {
        std::vector<InputEvent> filtered;
        for (const auto& event : result) {
            if (filter(event)) {
                filtered.push_back(event);
            }
        }
        result = filtered;
    }
    
    return result;
}

void EventFilter::clearFilters() {
    filters.clear();
}

void EventFilter::clearFilter(size_t index) {
    if (index < filters.size()) {
        filters.erase(filters.begin() + index);
    }
}

size_t EventFilter::getFilterCount() const {
    return filters.size();
}

} // namespace input
} // namespace knulli

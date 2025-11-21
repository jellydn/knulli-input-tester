#ifndef KNULLI_INPUT_EVENT_FILTER_H
#define KNULLI_INPUT_EVENT_FILTER_H

#include "input_event.h"
#include "event_history.h"
#include <vector>
#include <functional>

namespace knulli {
namespace input {

class EventFilter {
public:
    EventFilter();
    ~EventFilter();
    
    // Filter predicates
    void filterByDeviceId(int deviceId);
    void filterByEventType(InputType type);
    void filterByEventCode(InputCode code);
    void filterByDeviceIdRange(int minId, int maxId);
    
    // Custom filter
    void setCustomFilter(std::function<bool(const InputEvent&)> predicate);
    
    // Apply filter
    std::vector<InputEvent> apply(const EventHistory& history) const;
    
    // Clear filters
    void clearFilters();
    void clearFilter(size_t index);
    
    // Filter state
    size_t getFilterCount() const;

private:
    std::vector<std::function<bool(const InputEvent&)>> filters;
};

} // namespace input
} // namespace knulli

#endif // KNULLI_INPUT_EVENT_FILTER_H

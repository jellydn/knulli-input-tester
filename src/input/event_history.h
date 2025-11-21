#ifndef KNULLI_INPUT_EVENT_HISTORY_H
#define KNULLI_INPUT_EVENT_HISTORY_H

#include "input_event.h"
#include <deque>
#include <vector>
#include <shared_mutex>

namespace knulli {
namespace input {

class EventHistory {
public:
    static constexpr size_t MAX_EVENTS = 1000;
    
    EventHistory();
    ~EventHistory();
    
    void addEvent(const InputEvent& event);
    std::vector<InputEvent> getEvents(size_t count = MAX_EVENTS) const;
    std::vector<InputEvent> getEventsForDevice(int deviceId) const;
    std::vector<InputEvent> getEventsInTimeRange(
        std::chrono::milliseconds start, 
        std::chrono::milliseconds end
    ) const;
    
    void clear();
    size_t size() const;
    bool isEmpty() const;
    
    // History access
    const InputEvent* getEventAt(size_t index) const;
    const InputEvent* getLatestEvent() const;

private:
    std::deque<InputEvent> events;
    mutable std::shared_mutex mutex;
};

} // namespace input
} // namespace knulli

#endif // KNULLI_INPUT_EVENT_HISTORY_H

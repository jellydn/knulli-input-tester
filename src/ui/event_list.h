#ifndef KNULLI_UI_EVENT_LIST_H
#define KNULLI_UI_EVENT_LIST_H

#include "../input/input_event.h"
#include <vector>

namespace knulli {
namespace ui {

class EventListWidget {
public:
    EventListWidget();
    ~EventListWidget();
    
    // Rendering
    void render();
    void update(const std::vector<knulli::input::InputEvent>& events);
    
    // Configuration
    void setMaxVisibleItems(size_t count);
    void setShowTimestamps(bool show);
    void setShowDeviceNames(bool show);
    
    // State
    size_t getVisibleItemCount() const;
    bool isEmpty() const;
    
    // Scrolling
    void scrollToBottom();
    void scrollToTop();
    int getScrollPosition() const;
    void setScrollPosition(int pos);

private:
    const char* getCodeName(knulli::input::InputCode code);
    
    std::vector<knulli::input::InputEvent> displayedEvents;
    size_t maxVisibleItems;
    int scrollPosition;
    bool showTimestamps;
    bool showDeviceNames;
};

} // namespace ui
} // namespace knulli

#endif // KNULLI_UI_EVENT_LIST_H

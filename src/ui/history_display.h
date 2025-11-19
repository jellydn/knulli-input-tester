#ifndef KNULLI_UI_HISTORY_DISPLAY_H
#define KNULLI_UI_HISTORY_DISPLAY_H

#include "../input/event_history.h"
#include "../input/event_filter.h"
#include <vector>
#include <string>

namespace knulli {
namespace ui {

class HistoryDisplayWidget {
public:
    HistoryDisplayWidget();
    ~HistoryDisplayWidget();
    
    // Rendering
    void render();
    void update(const input::EventHistory& history);
    
    // Filtering
    void applyFilter(const input::EventFilter& filter);
    void clearFilter();
    
    // Navigation
    void scrollUp();
    void scrollDown();
    void scrollToTop();
    void scrollToBottom();
    void setScrollPosition(int position);
    
    // Configuration
    void setShowTimestamps(bool show);
    void setShowDeviceNames(bool show);
    void setMaxDisplayedItems(size_t count);
    
    // Export
    std::string exportToJSON() const;
    std::string exportToCSV() const;
    bool exportToFile(const std::string& filename, const std::string& format);
    
    // State
    size_t getTotalEvents() const;
    size_t getDisplayedEventCount() const;

private:
    std::vector<input::InputEvent> displayedEvents;
    std::vector<input::InputEvent> filteredEvents;
    int scrollPosition;
    size_t maxDisplayedItems;
    bool showTimestamps;
    bool showDeviceNames;
};

} // namespace ui
} // namespace knulli

#endif // KNULLI_UI_HISTORY_DISPLAY_H

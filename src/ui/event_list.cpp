#include "event_list.h"
#include "../core/logger.h"

namespace knulli {
namespace ui {

EventListWidget::EventListWidget()
    : maxVisibleItems(20), scrollPosition(0), 
      showTimestamps(true), showDeviceNames(true) {
    core::Logger::getInstance().debug("EventListWidget created");
}

EventListWidget::~EventListWidget() {
    displayedEvents.clear();
}

void EventListWidget::render() {
    // Render event list widget using ImGui
    // ImGui::Begin("Input Events");
    // ... render table/list of events
    // ImGui::End();
}

void EventListWidget::update(const std::vector<knulli::input::InputEvent>& events) {
    displayedEvents = events;
    
    // Keep only the latest maxVisibleItems
    if (displayedEvents.size() > maxVisibleItems) {
        displayedEvents.erase(
            displayedEvents.begin(),
            displayedEvents.begin() + (displayedEvents.size() - maxVisibleItems)
        );
    }
    
    // Auto-scroll to bottom
    scrollToBottom();
}

void EventListWidget::setMaxVisibleItems(size_t count) {
    maxVisibleItems = count;
}

void EventListWidget::setShowTimestamps(bool show) {
    showTimestamps = show;
}

void EventListWidget::setShowDeviceNames(bool show) {
    showDeviceNames = show;
}

size_t EventListWidget::getVisibleItemCount() const {
    return displayedEvents.size();
}

bool EventListWidget::isEmpty() const {
    return displayedEvents.empty();
}

void EventListWidget::scrollToBottom() {
    scrollPosition = static_cast<int>(displayedEvents.size()) - static_cast<int>(maxVisibleItems);
    if (scrollPosition < 0) {
        scrollPosition = 0;
    }
}

void EventListWidget::scrollToTop() {
    scrollPosition = 0;
}

int EventListWidget::getScrollPosition() const {
    return scrollPosition;
}

void EventListWidget::setScrollPosition(int pos) {
    scrollPosition = pos;
}

} // namespace ui
} // namespace knulli

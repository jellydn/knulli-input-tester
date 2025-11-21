#include "event_list.h"
#include "../core/logger.h"
#include <iomanip>
#include <sstream>
#include <chrono>

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
    // Currently stubbed - rendering will be handled by main display with SDL2
    // Event data is stored in displayedEvents for use by display system
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

const char* EventListWidget::getCodeName(knulli::input::InputCode code) {
    switch (code) {
        // Face buttons
        case knulli::input::InputCode::ButtonA:
            return "Button A";
        case knulli::input::InputCode::ButtonB:
            return "Button B";
        case knulli::input::InputCode::ButtonX:
            return "Button X";
        case knulli::input::InputCode::ButtonY:
            return "Button Y";
        
        // Control buttons
        case knulli::input::InputCode::ButtonStart:
            return "Start";
        case knulli::input::InputCode::ButtonSelect:
            return "Select";
        case knulli::input::InputCode::ButtonFunction:
            return "Function";
        
        // D-Pad
        case knulli::input::InputCode::DPadUp:
            return "D-Pad Up";
        case knulli::input::InputCode::DPadDown:
            return "D-Pad Down";
        case knulli::input::InputCode::DPadLeft:
            return "D-Pad Left";
        case knulli::input::InputCode::DPadRight:
            return "D-Pad Right";
        
        // Axes
        case knulli::input::InputCode::AxisLeftX:
            return "Left Stick X";
        case knulli::input::InputCode::AxisLeftY:
            return "Left Stick Y";
        case knulli::input::InputCode::AxisRightX:
            return "Right Stick X";
        case knulli::input::InputCode::AxisRightY:
            return "Right Stick Y";
        
        // Triggers
        case knulli::input::InputCode::TriggerLeft:
            return "L Trigger";
        case knulli::input::InputCode::TriggerRight:
            return "R Trigger";
        
        // Shoulder buttons
        case knulli::input::InputCode::ShoulderLeft:
            return "LB";
        case knulli::input::InputCode::ShoulderRight:
            return "RB";
        
        // Special
        case knulli::input::InputCode::PowerButton:
            return "Power";
        case knulli::input::InputCode::ToggleSwitch:
            return "Toggle";
        case knulli::input::InputCode::Unknown:
            return "Unknown";
    }
    return "Unknown";
}

} // namespace ui
} // namespace knulli

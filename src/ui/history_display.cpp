#include "history_display.h"
#include "../core/logger.h"
#include <fstream>
#include <sstream>
#include <iomanip>

namespace knulli {
namespace ui {

HistoryDisplayWidget::HistoryDisplayWidget()
    : scrollPosition(0), maxDisplayedItems(100),
      showTimestamps(true), showDeviceNames(true) {
    core::Logger::getInstance().debug("HistoryDisplayWidget created");
}

HistoryDisplayWidget::~HistoryDisplayWidget() {
}

void HistoryDisplayWidget::render() {
    // Render history display using ImGui
    // ImGui::Begin("Input History");
    // ... render scrollable list of events
    // ImGui::End();
}

void HistoryDisplayWidget::update(const input::EventHistory& history) {
    displayedEvents = history.getEvents(maxDisplayedItems);
    filteredEvents = displayedEvents;
}

void HistoryDisplayWidget::applyFilter(const input::EventFilter& filter) {
    filteredEvents = filter.apply(const_cast<input::EventHistory&>(
        *reinterpret_cast<const input::EventHistory*>(nullptr)
    ));
}

void HistoryDisplayWidget::clearFilter() {
    filteredEvents = displayedEvents;
}

void HistoryDisplayWidget::scrollUp() {
    if (scrollPosition > 0) {
        scrollPosition--;
    }
}

void HistoryDisplayWidget::scrollDown() {
    scrollPosition++;
}

void HistoryDisplayWidget::scrollToTop() {
    scrollPosition = 0;
}

void HistoryDisplayWidget::scrollToBottom() {
    if (filteredEvents.size() > maxDisplayedItems) {
        scrollPosition = filteredEvents.size() - maxDisplayedItems;
    } else {
        scrollPosition = 0;
    }
}

void HistoryDisplayWidget::setScrollPosition(int position) {
    scrollPosition = std::max(0, position);
}

void HistoryDisplayWidget::setShowTimestamps(bool show) {
    showTimestamps = show;
}

void HistoryDisplayWidget::setShowDeviceNames(bool show) {
    showDeviceNames = show;
}

void HistoryDisplayWidget::setMaxDisplayedItems(size_t count) {
    maxDisplayedItems = count;
}

std::string HistoryDisplayWidget::exportToJSON() const {
    std::ostringstream oss;
    oss << "{\n  \"events\": [\n";
    
    for (size_t i = 0; i < filteredEvents.size(); ++i) {
        const auto& event = filteredEvents[i];
        oss << "    {\n";
        oss << "      \"device_id\": " << event.deviceId << ",\n";
        oss << "      \"device_name\": \"" << event.deviceName << "\",\n";
        oss << "      \"description\": \"" << event.description << "\",\n";
        oss << "      \"value\": " << event.value << "\n";
        oss << "    }";
        if (i < filteredEvents.size() - 1) {
            oss << ",";
        }
        oss << "\n";
    }
    
    oss << "  ]\n}";
    return oss.str();
}

std::string HistoryDisplayWidget::exportToCSV() const {
    std::ostringstream oss;
    oss << "Device ID,Device Name,Description,Value\n";
    
    for (const auto& event : filteredEvents) {
        oss << event.deviceId << ",";
        oss << event.deviceName << ",";
        oss << event.description << ",";
        oss << event.value << "\n";
    }
    
    return oss.str();
}

bool HistoryDisplayWidget::exportToFile(const std::string& filename, const std::string& format) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        core::Logger::getInstance().error("Cannot open file for export: " + filename);
        return false;
    }
    
    if (format == "json") {
        file << exportToJSON();
    } else if (format == "csv") {
        file << exportToCSV();
    } else {
        core::Logger::getInstance().warning("Unknown export format: " + format);
        return false;
    }
    
    file.close();
    core::Logger::getInstance().info("History exported to: " + filename);
    return true;
}

size_t HistoryDisplayWidget::getTotalEvents() const {
    return displayedEvents.size();
}

size_t HistoryDisplayWidget::getDisplayedEventCount() const {
    return filteredEvents.size();
}

} // namespace ui
} // namespace knulli

#ifndef KNULLI_UI_BUTTON_DISPLAY_H
#define KNULLI_UI_BUTTON_DISPLAY_H

#include "../input/input_event.h"

namespace knulli {
namespace ui {

class ButtonDisplay {
public:
    static std::string getButtonLabel(input::InputCode code);
    static std::string getEventTypeLabel(input::InputType type);
    
    // Visual styling for events
    static void setButtonPressColor(float r, float g, float b, float a = 1.0f);
    static void setButtonReleaseColor(float r, float g, float b, float a = 1.0f);
    
    // Display formatting
    static std::string formatEventForDisplay(const input::InputEvent& event);
    static std::string formatTimestamp(const std::chrono::high_resolution_clock::time_point& time);
};

} // namespace ui
} // namespace knulli

#endif // KNULLI_UI_BUTTON_DISPLAY_H

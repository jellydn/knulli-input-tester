#include "button_display.h"
#include <chrono>
#include <iomanip>
#include <sstream>

namespace knulli {
namespace ui {

static float buttonPressR = 0.0f, buttonPressG = 1.0f, buttonPressB = 0.0f, buttonPressA = 1.0f;
static float buttonReleaseR = 1.0f, buttonReleaseG = 0.0f, buttonReleaseB = 0.0f, buttonReleaseA = 1.0f;

std::string ButtonDisplay::getButtonLabel(input::InputCode code) {
    switch (code) {
        case input::InputCode::ButtonA: return "A (East)";
        case input::InputCode::ButtonB: return "B (South)";
        case input::InputCode::ButtonX: return "X (North)";
        case input::InputCode::ButtonY: return "Y (West)";
        case input::InputCode::ButtonStart: return "Start";
        case input::InputCode::ButtonSelect: return "Select";
        case input::InputCode::ButtonFunction: return "Function";
        case input::InputCode::DPadUp: return "D-Pad Up";
        case input::InputCode::DPadDown: return "D-Pad Down";
        case input::InputCode::DPadLeft: return "D-Pad Left";
        case input::InputCode::DPadRight: return "D-Pad Right";
        case input::InputCode::AxisLeftX: return "Left Stick X";
        case input::InputCode::AxisLeftY: return "Left Stick Y";
        case input::InputCode::AxisRightX: return "Right Stick X";
        case input::InputCode::AxisRightY: return "Right Stick Y";
        case input::InputCode::TriggerLeft: return "L Trigger";
        case input::InputCode::TriggerRight: return "R Trigger";
        case input::InputCode::ShoulderLeft: return "L Shoulder";
        case input::InputCode::ShoulderRight: return "R Shoulder";
        case input::InputCode::PowerButton: return "Power";
        case input::InputCode::ToggleSwitch: return "Toggle";
        default: return "Unknown";
    }
}

std::string ButtonDisplay::getEventTypeLabel(input::InputType type) {
    switch (type) {
        case input::InputType::ButtonPress: return "Press";
        case input::InputType::ButtonRelease: return "Release";
        case input::InputType::AxisMotion: return "Motion";
        case input::InputType::HatSwitch: return "Hat";
        case input::InputType::DeviceConnect: return "Connect";
        case input::InputType::DeviceDisconnect: return "Disconnect";
        default: return "Unknown";
    }
}

void ButtonDisplay::setButtonPressColor(float r, float g, float b, float a) {
    buttonPressR = r;
    buttonPressG = g;
    buttonPressB = b;
    buttonPressA = a;
}

void ButtonDisplay::setButtonReleaseColor(float r, float g, float b, float a) {
    buttonReleaseR = r;
    buttonReleaseG = g;
    buttonReleaseB = b;
    buttonReleaseA = a;
}

std::string ButtonDisplay::formatEventForDisplay(const input::InputEvent& event) {
    std::ostringstream oss;
    oss << "[" << event.deviceName << "] ";
    oss << getEventTypeLabel(event.type) << " - ";
    oss << getButtonLabel(event.code);
    
    if (event.type == input::InputType::AxisMotion) {
        oss << " (" << event.value << ")";
    }
    
    return oss.str();
}

std::string ButtonDisplay::formatTimestamp(const std::chrono::high_resolution_clock::time_point& time) {
    auto duration = time.time_since_epoch();
    auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
    
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(3) << (millis / 1000.0f);
    return oss.str() + "s";
}

} // namespace ui
} // namespace knulli

#ifndef KNULLI_INPUT_EVENT_H
#define KNULLI_INPUT_EVENT_H

#include <chrono>
#include <string>

namespace knulli {
namespace input {

enum class InputType {
    ButtonPress,
    ButtonRelease,
    AxisMotion,
    HatSwitch,
    DeviceConnect,
    DeviceDisconnect
};

enum class InputCode {
    // Face buttons (KNULLI mapping)
    ButtonA,      // East, ○ on PlayStation
    ButtonB,      // South, ✕ on PlayStation  
    ButtonX,      // North, △ on PlayStation
    ButtonY,      // West, ◻ on PlayStation
    
    // Control buttons
    ButtonStart,
    ButtonSelect,
    ButtonFunction, // KNULLI hotkey button
    
    // D-pad and axes
    DPadUp, DPadDown, DPadLeft, DPadRight,
    AxisLeftX, AxisLeftY, AxisRightX, AxisRightY,
    
    // Triggers and shoulders
    TriggerLeft, TriggerRight,
    ShoulderLeft, ShoulderRight,
    
    // Special
    PowerButton,
    ToggleSwitch,
    Unknown
};

struct InputEvent {
    std::chrono::high_resolution_clock::time_point timestamp;
    int deviceId;
    InputType type;
    InputCode code;
    int value;
    std::string deviceName;
    std::string description;
    
    InputEvent() 
        : deviceId(0), type(InputType::ButtonPress), code(InputCode::Unknown), value(0) {
        timestamp = std::chrono::high_resolution_clock::now();
    }
};

} // namespace input
} // namespace knulli

#endif // KNULLI_INPUT_EVENT_H

#ifndef KNULLI_INPUT_DEVICE_H
#define KNULLI_INPUT_DEVICE_H

#include "input_event.h"
#include <string>
#include <memory>
#include <vector>

namespace knulli {
namespace input {

enum class ConnectionType {
    BuiltIn,       // Device's built-in controls
    USB,           // USB connected controller
    Bluetooth,     // Bluetooth controller
    Unknown
};

enum class DeviceState {
    Connected,
    Disconnected,
    Error,
    Initializing
};

struct DeviceCapabilities {
    bool hasButtons = false;
    bool hasAxes = false;
    bool hasHats = false;
    int buttonCount = 0;
    int axisCount = 0;
    int hatCount = 0;
    bool supportsRumble = false;
    bool supportsGyro = false;
};

class InputDevice {
public:
    InputDevice(int id, const std::string& name);
    virtual ~InputDevice();
    
    // Device identification
    int getDeviceId() const { return deviceId; }
    std::string getName() const { return name; }
    std::string getUniqueId() const { return uniqueId; }
    
    // State management
    void setState(DeviceState newState) { state = newState; }
    DeviceState getState() const { return state; }
    
    // Connection type
    void setConnectionType(ConnectionType connType) { connectionType = connType; }
    ConnectionType getConnectionType() const { return connectionType; }
    
    // Capabilities
    void setCapabilities(const DeviceCapabilities& caps) { capabilities = caps; }
    DeviceCapabilities getCapabilities() const { return capabilities; }
    
    // Event handling
    virtual bool hasPendingEvents() const { return false; }
    virtual InputEvent getNextEvent() { return InputEvent(); }
    
    // Activity tracking
    void recordActivity() { 
        lastActivity = std::chrono::steady_clock::now();
    }
    std::chrono::steady_clock::time_point getLastActivity() const {
        return lastActivity;
    }
    
    // Path management
    void setPhysicalPath(const std::string& path) { physicalPath = path; }
    std::string getPhysicalPath() const { return physicalPath; }

protected:
    int deviceId;
    std::string name;
    std::string uniqueId;
    std::string physicalPath;
    ConnectionType connectionType;
    DeviceCapabilities capabilities;
    DeviceState state;
    std::chrono::steady_clock::time_point lastActivity;

private:
    InputDevice(const InputDevice&) = delete;
    InputDevice& operator=(const InputDevice&) = delete;
};

} // namespace input
} // namespace knulli

#endif // KNULLI_INPUT_DEVICE_H

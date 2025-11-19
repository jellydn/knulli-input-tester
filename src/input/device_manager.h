#ifndef KNULLI_INPUT_DEVICE_MANAGER_H
#define KNULLI_INPUT_DEVICE_MANAGER_H

#include "input_device.h"
#include "input_event.h"
#include <vector>
#include <memory>
#include <functional>
#include <shared_mutex>

namespace knulli {
namespace input {

class InputManager {
public:
    static InputManager& getInstance();
    
    // Device management
    void detectDevices();
    void addDevice(std::unique_ptr<InputDevice> device);
    void removeDevice(int deviceId);
    
    // Device queries
    InputDevice* getDevice(int deviceId);
    const InputDevice* getDevice(int deviceId) const;
    std::vector<InputDevice*> getAllDevices();
    size_t getDeviceCount() const;
    
    // Event processing
    void processEvents();
    void registerEventCallback(std::function<void(const InputEvent&)> callback);
    
    // Input monitoring
    bool startInputMonitoring();
    void stopInputMonitoring();
    bool isMonitoring() const;
    
    // Device hot-plug detection
    void startDeviceMonitoring();
    void stopDeviceMonitoring();

private:
    InputManager();
    ~InputManager();
    InputManager(const InputManager&) = delete;
    InputManager& operator=(const InputManager&) = delete;
    
    std::vector<std::unique_ptr<InputDevice>> devices;
    std::vector<std::function<void(const InputEvent&)>> eventCallbacks;
    mutable std::shared_mutex devicesMutex;
    mutable std::shared_mutex callbacksMutex;
    bool monitoring;
    bool deviceMonitoring;
};

} // namespace input
} // namespace knulli

#endif // KNULLI_INPUT_DEVICE_MANAGER_H

#include "device_manager.h"
#include "../core/logger.h"
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>

namespace knulli {
namespace input {

InputManager& InputManager::getInstance() {
    static InputManager instance;
    return instance;
}

InputManager::InputManager()
    : monitoring(false), deviceMonitoring(false) {
    core::Logger::getInstance().info("InputManager initialized");
}

InputManager::~InputManager() {
    stopInputMonitoring();
    stopDeviceMonitoring();
    devices.clear();
}

void InputManager::detectDevices() {
    core::Logger::getInstance().info("Detecting input devices");
    
    const char* inputDevicePath = "/dev/input";
    DIR* dir = opendir(inputDevicePath);
    if (!dir) {
        core::Logger::getInstance().error("Cannot open /dev/input");
        return;
    }
    
    struct dirent* entry;
    int deviceId = 0;
    
    while ((entry = readdir(dir)) != nullptr) {
        if (strncmp(entry->d_name, "event", 5) == 0) {
            std::string devicePath = std::string(inputDevicePath) + "/" + entry->d_name;
            
            auto device = std::make_unique<InputDevice>(deviceId++, entry->d_name);
            device->setPhysicalPath(devicePath);
            device->setState(DeviceState::Connected);
            
            // Detect connection type based on path
            if (devicePath.find("platform") != std::string::npos) {
                device->setConnectionType(ConnectionType::BuiltIn);
            } else if (devicePath.find("usb") != std::string::npos) {
                device->setConnectionType(ConnectionType::USB);
            } else if (devicePath.find("bluetooth") != std::string::npos) {
                device->setConnectionType(ConnectionType::Bluetooth);
            }
            
            // Initialize basic capabilities
            DeviceCapabilities caps;
            caps.hasButtons = true;
            caps.hasAxes = true;
            caps.buttonCount = 16;
            caps.axisCount = 6;
            device->setCapabilities(caps);
            
            devices.push_back(std::move(device));
            core::Logger::getInstance().debug("Device detected: " + devicePath);
        }
    }
    
    closedir(dir);
    core::Logger::getInstance().info("Device detection complete. Found " + std::to_string(devices.size()) + " devices");
}

void InputManager::addDevice(std::unique_ptr<InputDevice> device) {
    std::unique_lock lock(devicesMutex);
    devices.push_back(std::move(device));
    core::Logger::getInstance().info("Device added: " + devices.back()->getName());
}

void InputManager::removeDevice(int deviceId) {
    std::unique_lock lock(devicesMutex);
    
    auto it = std::find_if(devices.begin(), devices.end(),
        [deviceId](const std::unique_ptr<InputDevice>& dev) {
            return dev->getDeviceId() == deviceId;
        });
    
    if (it != devices.end()) {
        core::Logger::getInstance().info("Device removed: " + (*it)->getName());
        devices.erase(it);
    }
}

InputDevice* InputManager::getDevice(int deviceId) {
    std::shared_lock lock(devicesMutex);
    
    auto it = std::find_if(devices.begin(), devices.end(),
        [deviceId](const std::unique_ptr<InputDevice>& dev) {
            return dev->getDeviceId() == deviceId;
        });
    
    if (it != devices.end()) {
        return it->get();
    }
    return nullptr;
}

const InputDevice* InputManager::getDevice(int deviceId) const {
    std::shared_lock lock(devicesMutex);
    
    auto it = std::find_if(devices.begin(), devices.end(),
        [deviceId](const std::unique_ptr<InputDevice>& dev) {
            return dev->getDeviceId() == deviceId;
        });
    
    if (it != devices.end()) {
        return it->get();
    }
    return nullptr;
}

std::vector<InputDevice*> InputManager::getAllDevices() {
    std::shared_lock lock(devicesMutex);
    
    std::vector<InputDevice*> deviceList;
    for (const auto& device : devices) {
        deviceList.push_back(device.get());
    }
    return deviceList;
}

size_t InputManager::getDeviceCount() const {
    std::shared_lock lock(devicesMutex);
    return devices.size();
}

void InputManager::processEvents() {
    std::shared_lock lock(devicesMutex);
    
    for (const auto& device : devices) {
        if (device->getState() != DeviceState::Connected) {
            continue;
        }
        
        while (device->hasPendingEvents()) {
            InputEvent event = device->getNextEvent();
            
            // Invoke all registered callbacks
            {
                std::shared_lock cbLock(callbacksMutex);
                for (const auto& callback : eventCallbacks) {
                    callback(event);
                }
            }
        }
    }
}

void InputManager::registerEventCallback(std::function<void(const InputEvent&)> callback) {
    std::unique_lock lock(callbacksMutex);
    eventCallbacks.push_back(callback);
}

bool InputManager::startInputMonitoring() {
    if (monitoring) {
        return true;
    }
    
    monitoring = true;
    core::Logger::getInstance().info("Input monitoring started");
    return true;
}

void InputManager::stopInputMonitoring() {
    if (monitoring) {
        monitoring = false;
        core::Logger::getInstance().info("Input monitoring stopped");
    }
}

bool InputManager::isMonitoring() const {
    return monitoring;
}

void InputManager::startDeviceMonitoring() {
    if (deviceMonitoring) {
        return;
    }
    
    deviceMonitoring = true;
    core::Logger::getInstance().info("Device monitoring started");
}

void InputManager::stopDeviceMonitoring() {
    if (deviceMonitoring) {
        deviceMonitoring = false;
        core::Logger::getInstance().info("Device monitoring stopped");
    }
}

} // namespace input
} // namespace knulli

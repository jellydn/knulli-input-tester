#include "device_identifier.h"
#include "../core/logger.h"

namespace knulli {
namespace input {

DeviceIdentifier& DeviceIdentifier::getInstance() {
    static DeviceIdentifier instance;
    return instance;
}

DeviceIdentifier::DeviceIdentifier() {
    core::Logger::getInstance().info("DeviceIdentifier initialized");
}

DeviceIdentifier::~DeviceIdentifier() {
}

DeviceIdentification DeviceIdentifier::identifyDevice(InputDevice* device) {
    if (!device) {
        return DeviceIdentification{};
    }
    
    DeviceIdentification ident;
    ident.deviceId = device->getDeviceId();
    ident.name = device->getName();
    ident.connectionType = device->getConnectionType();
    
    // Auto-generate label based on device count
    int deviceCount = identifications.size();
    char label[32];
    snprintf(label, sizeof(label), "Device %d", deviceCount + 1);
    ident.label = label;
    
    // Assign color based on device count (cycle through available colors)
    if (deviceCount < 4) {
        ident.color = Color(COLORS[deviceCount][0], COLORS[deviceCount][1], 
                           COLORS[deviceCount][2], COLORS[deviceCount][3]);
    } else {
        ident.color = Color(0.5f, 0.5f, 0.5f, 1.0f);  // Default gray
    }
    
    identifications[device->getDeviceId()] = ident;
    core::Logger::getInstance().debug("Device identified: " + device->getName());
    
    return ident;
}

void DeviceIdentifier::assignLabel(int deviceId, const std::string& label) {
    auto it = identifications.find(deviceId);
    if (it != identifications.end()) {
        it->second.label = label;
        core::Logger::getInstance().debug("Label assigned to device " + std::to_string(deviceId) + ": " + label);
    }
}

void DeviceIdentifier::assignColor(int deviceId, float r, float g, float b, float a) {
    auto it = identifications.find(deviceId);
    if (it != identifications.end()) {
        it->second.color = Color(r, g, b, a);
    }
}

DeviceIdentification DeviceIdentifier::getIdentification(int deviceId) const {
    auto it = identifications.find(deviceId);
    if (it != identifications.end()) {
        return it->second;
    }
    return DeviceIdentification{};
}

std::string DeviceIdentifier::getDeviceLabel(int deviceId) const {
    auto it = identifications.find(deviceId);
    if (it != identifications.end()) {
        return it->second.label;
    }
    return "Unknown";
}

Color DeviceIdentifier::getDeviceColor(int deviceId) const {
    auto it = identifications.find(deviceId);
    if (it != identifications.end()) {
        return it->second.color;
    }
    return Color(0.5f, 0.5f, 0.5f, 1.0f);
}

ConnectionType DeviceIdentifier::detectConnectionType(const std::string& devicePath) {
    if (devicePath.find("platform") != std::string::npos) {
        return ConnectionType::BuiltIn;
    } else if (devicePath.find("usb") != std::string::npos) {
        return ConnectionType::USB;
    } else if (devicePath.find("bluetooth") != std::string::npos) {
        return ConnectionType::Bluetooth;
    }
    return ConnectionType::Unknown;
}

std::string DeviceIdentifier::getDeviceInfo(int deviceId) const {
    auto it = identifications.find(deviceId);
    if (it != identifications.end()) {
        const auto& info = it->second;
        std::string result = info.label + " (" + info.name + ")";
        return result;
    }
    return "Unknown device";
}

} // namespace input
} // namespace knulli

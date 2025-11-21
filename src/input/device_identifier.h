#ifndef KNULLI_INPUT_DEVICE_IDENTIFIER_H
#define KNULLI_INPUT_DEVICE_IDENTIFIER_H

#include "input_device.h"
#include <string>
#include <map>

namespace knulli {
namespace input {

struct Color {
    float r, g, b, a;
    Color(float _r = 1.0f, float _g = 1.0f, float _b = 1.0f, float _a = 1.0f)
        : r(_r), g(_g), b(_b), a(_a) {}
};

struct DeviceIdentification {
    int deviceId;
    std::string name;
    std::string label;
    Color color;
    ConnectionType connectionType;
    std::string manufacturer;
    std::string model;
};

class DeviceIdentifier {
public:
    static DeviceIdentifier& getInstance();
    
    // Device identification
    DeviceIdentification identifyDevice(InputDevice* device);
    void assignLabel(int deviceId, const std::string& label);
    void assignColor(int deviceId, float r, float g, float b, float a = 1.0f);
    
    // Lookup
    DeviceIdentification getIdentification(int deviceId) const;
    std::string getDeviceLabel(int deviceId) const;
    Color getDeviceColor(int deviceId) const;
    
    // Connection type detection
    ConnectionType detectConnectionType(const std::string& devicePath);
    
    // Device info
    std::string getDeviceInfo(int deviceId) const;

private:
    DeviceIdentifier();
    ~DeviceIdentifier();
    DeviceIdentifier(const DeviceIdentifier&) = delete;
    DeviceIdentifier& operator=(const DeviceIdentifier&) = delete;
    
    std::map<int, DeviceIdentification> identifications;
    static constexpr float COLORS[][4] = {
        {0.2f, 0.8f, 0.2f, 1.0f},  // Green
        {0.2f, 0.2f, 0.8f, 1.0f},  // Blue
        {0.8f, 0.2f, 0.2f, 1.0f},  // Red
        {0.8f, 0.8f, 0.2f, 1.0f},  // Yellow
    };
};

} // namespace input
} // namespace knulli

#endif // KNULLI_INPUT_DEVICE_IDENTIFIER_H

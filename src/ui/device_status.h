#ifndef KNULLI_UI_DEVICE_STATUS_H
#define KNULLI_UI_DEVICE_STATUS_H

#include "../input/device_manager.h"
#include "../input/device_identifier.h"
#include <vector>

namespace knulli {
namespace ui {

class DeviceStatusWidget {
public:
    DeviceStatusWidget();
    ~DeviceStatusWidget();
    
    // Rendering
    void render();
    void update();
    
    // Configuration
    void setShowCapabilities(bool show);
    void setShowConnectionType(bool show);
    
    // State
    int getSelectedDeviceId() const;
    void setSelectedDeviceId(int deviceId);

private:
    std::vector<int> connectedDevices;
    int selectedDeviceId;
    bool showCapabilities;
    bool showConnectionType;
    
    input::InputManager& inputManager;
    input::DeviceIdentifier& deviceIdentifier;
};

} // namespace ui
} // namespace knulli

#endif // KNULLI_UI_DEVICE_STATUS_H

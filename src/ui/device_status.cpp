#include "device_status.h"
#include "../core/logger.h"

namespace knulli {
namespace ui {

DeviceStatusWidget::DeviceStatusWidget()
    : selectedDeviceId(-1), showCapabilities(true), showConnectionType(true),
      inputManager(input::InputManager::getInstance()),
      deviceIdentifier(input::DeviceIdentifier::getInstance()) {
    core::Logger::getInstance().debug("DeviceStatusWidget created");
}

DeviceStatusWidget::~DeviceStatusWidget() {
}

void DeviceStatusWidget::render() {
    // Render device status panel using ImGui
    // ImGui::Begin("Connected Devices");
    // For each device: show label, connection type, capabilities
    // ImGui::End();
}

void DeviceStatusWidget::update() {
    connectedDevices.clear();
    
    auto devices = inputManager.getAllDevices();
    for (const auto& device : devices) {
        if (device->getState() == input::DeviceState::Connected) {
            connectedDevices.push_back(device->getDeviceId());
            
            // Identify device if not already done
            deviceIdentifier.identifyDevice(device);
        }
    }
}

void DeviceStatusWidget::setShowCapabilities(bool show) {
    showCapabilities = show;
}

void DeviceStatusWidget::setShowConnectionType(bool show) {
    showConnectionType = show;
}

int DeviceStatusWidget::getSelectedDeviceId() const {
    return selectedDeviceId;
}

void DeviceStatusWidget::setSelectedDeviceId(int deviceId) {
    selectedDeviceId = deviceId;
}

} // namespace ui
} // namespace knulli

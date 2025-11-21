#include "input_device.h"
#include <uuid/uuid.h>
#include <sstream>
#include <iomanip>

namespace knulli {
namespace input {

InputDevice::InputDevice(int id, const std::string& deviceName)
    : deviceId(id), name(deviceName), connectionType(ConnectionType::Unknown),
      state(DeviceState::Initializing) {
    
    // Generate unique ID using UUID
    uuid_t uuid;
    uuid_generate(uuid);
    char uuid_str[37];
    uuid_unparse(uuid, uuid_str);
    uniqueId = std::string(uuid_str);
    
    lastActivity = std::chrono::steady_clock::now();
    
    // Initialize capabilities to empty
    capabilities = DeviceCapabilities();
}

InputDevice::~InputDevice() {
}

} // namespace input
} // namespace knulli

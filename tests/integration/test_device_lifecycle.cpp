#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include "../../src/input/device_manager.h"
#include "../../src/input/input_device.h"

using namespace knulli::input;

SCENARIO("Devices can be hot-plugged during runtime", "[device-lifecycle]") {
    GIVEN("Application is running with devices connected") {
        InputManager& manager = InputManager::getInstance();
        manager.startDeviceMonitoring();
        
        WHEN("new device is connected") {
            auto newDevice = std::make_unique<InputDevice>(4, "NewController");
            newDevice->setState(DeviceState::Connected);
            manager.addDevice(std::move(newDevice));
            
            THEN("new device is immediately available") {
                REQUIRE(manager.getDevice(4) != nullptr);
                REQUIRE(manager.getDevice(4)->getState() == DeviceState::Connected);
            }
        }
        
        WHEN("device is disconnected") {
            manager.removeDevice(4);
            
            THEN("device is no longer in device list") {
                REQUIRE(manager.getDevice(4) == nullptr);
            }
        }
    }
}

SCENARIO("Device state transitions work correctly", "[device-lifecycle]") {
    GIVEN("Input device") {
        auto device = std::make_unique<InputDevice>(5, "TestDevice");
        
        WHEN("device state changes") {
            device->setState(DeviceState::Initializing);
            REQUIRE(device->getState() == DeviceState::Initializing);
            
            device->setState(DeviceState::Connected);
            REQUIRE(device->getState() == DeviceState::Connected);
            
            device->setState(DeviceState::Error);
            REQUIRE(device->getState() == DeviceState::Error);
            
            device->setState(DeviceState::Disconnected);
            
            THEN("all transitions are valid") {
                REQUIRE(device->getState() == DeviceState::Disconnected);
            }
        }
    }
}

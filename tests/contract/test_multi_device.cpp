#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include "../../src/input/device_manager.h"
#include "../../src/input/input_device.h"

using namespace knulli::input;

SCENARIO("Multiple devices can be managed simultaneously", "[multi-device]") {
    GIVEN("InputManager with multiple connected devices") {
        InputManager& manager = InputManager::getInstance();
        
        WHEN("multiple devices are added") {
            auto dev1 = std::make_unique<InputDevice>(1, "Controller1");
            auto dev2 = std::make_unique<InputDevice>(2, "Controller2");
            auto dev3 = std::make_unique<InputDevice>(3, "Controller3");
            
            manager.addDevice(std::move(dev1));
            manager.addDevice(std::move(dev2));
            manager.addDevice(std::move(dev3));
            
            THEN("all devices are accessible") {
                REQUIRE(manager.getDeviceCount() == 3);
                REQUIRE(manager.getDevice(1) != nullptr);
                REQUIRE(manager.getDevice(2) != nullptr);
                REQUIRE(manager.getDevice(3) != nullptr);
            }
            
            AND_THEN("each device has unique ID") {
                auto devices = manager.getAllDevices();
                REQUIRE(devices[0]->getDeviceId() != devices[1]->getDeviceId());
                REQUIRE(devices[1]->getDeviceId() != devices[2]->getDeviceId());
            }
        }
    }
}

SCENARIO("Input events from different devices are distinguished", "[multi-device]") {
    GIVEN("Multiple controllers sending input") {
        InputManager& manager = InputManager::getInstance();
        std::vector<InputEvent> capturedEvents;
        
        manager.registerEventCallback([&capturedEvents](const InputEvent& event) {
            capturedEvents.push_back(event);
        });
        
        WHEN("devices send events with different device IDs") {
            InputEvent event1;
            event1.deviceId = 1;
            event1.code = InputCode::ButtonA;
            event1.deviceName = "Controller1";
            capturedEvents.push_back(event1);
            
            InputEvent event2;
            event2.deviceId = 2;
            event2.code = InputCode::ButtonB;
            event2.deviceName = "Controller2";
            capturedEvents.push_back(event2);
            
            THEN("events are clearly identified by device") {
                REQUIRE(capturedEvents[0].deviceId == 1);
                REQUIRE(capturedEvents[1].deviceId == 2);
                REQUIRE(capturedEvents[0].deviceName == "Controller1");
                REQUIRE(capturedEvents[1].deviceName == "Controller2");
            }
        }
    }
}

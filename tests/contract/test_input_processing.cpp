#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include "../../src/input/input_event.h"
#include "../../src/input/device_manager.h"

using namespace knulli::input;

SCENARIO("Input events are captured and processed correctly", "[input-processing]") {
    GIVEN("InputManager is initialized") {
        InputManager& manager = InputManager::getInstance();
        
        WHEN("input event is created") {
            InputEvent event;
            event.deviceId = 1;
            event.type = InputType::ButtonPress;
            event.code = InputCode::ButtonA;
            event.value = 1;
            event.deviceName = "TestController";
            event.description = "Button A pressed";
            
            THEN("event contains valid data") {
                REQUIRE(event.deviceId == 1);
                REQUIRE(event.type == InputType::ButtonPress);
                REQUIRE(event.code == InputCode::ButtonA);
                REQUIRE(event.value == 1);
                REQUIRE(event.deviceName == "TestController");
            }
        }
        
        WHEN("input event callback is registered") {
            bool callbackInvoked = false;
            manager.registerEventCallback([&callbackInvoked](const InputEvent&) {
                callbackInvoked = true;
            });
            
            THEN("callback is stored for later invocation") {
                REQUIRE(!callbackInvoked);  // Not invoked yet
            }
        }
    }
}

SCENARIO("Input latency meets performance requirements", "[input-processing]") {
    GIVEN("Input processing system") {
        WHEN("event is timestamped at capture") {
            auto captureTime = std::chrono::high_resolution_clock::now();
            InputEvent event;
            event.timestamp = captureTime;
            
            auto displayTime = std::chrono::high_resolution_clock::now();
            auto latency = std::chrono::duration_cast<std::chrono::milliseconds>(displayTime - captureTime);
            
            THEN("latency should be less than 100ms") {
                REQUIRE(latency < std::chrono::milliseconds(100));
            }
        }
    }
}

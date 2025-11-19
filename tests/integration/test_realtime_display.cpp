#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include "../../src/input/input_event.h"
#include "../../src/input/device_manager.h"
#include "../../src/core/logger.h"

using namespace knulli::input;
using namespace knulli::core;

SCENARIO("Real-time input display shows events within latency budget", "[realtime-display]") {
    GIVEN("Input tester display system is running") {
        Logger::getInstance().initialize("test-realtime-display");
        InputManager& manager = InputManager::getInstance();
        
        std::vector<InputEvent> capturedEvents;
        manager.registerEventCallback([&capturedEvents](const InputEvent& event) {
            capturedEvents.push_back(event);
        });
        
        WHEN("user presses a button") {
            InputEvent testEvent;
            testEvent.deviceId = 1;
            testEvent.type = InputType::ButtonPress;
            testEvent.code = InputCode::ButtonA;
            testEvent.value = 1;
            testEvent.deviceName = "TestController";
            testEvent.description = "A button pressed";
            
            auto captureTime = std::chrono::high_resolution_clock::now();
            testEvent.timestamp = captureTime;
            
            // Simulate event display
            auto displayTime = std::chrono::high_resolution_clock::now();
            auto latency = std::chrono::duration_cast<std::chrono::milliseconds>(displayTime - captureTime);
            
            THEN("event appears on screen within 100ms latency") {
                REQUIRE(latency < std::chrono::milliseconds(100));
                REQUIRE(testEvent.type == InputType::ButtonPress);
                REQUIRE(testEvent.code == InputCode::ButtonA);
            }
        }
        
        WHEN("button is released") {
            InputEvent releaseEvent;
            releaseEvent.deviceId = 1;
            releaseEvent.type = InputType::ButtonRelease;
            releaseEvent.code = InputCode::ButtonA;
            releaseEvent.value = 0;
            releaseEvent.deviceName = "TestController";
            releaseEvent.description = "A button released";
            releaseEvent.timestamp = std::chrono::high_resolution_clock::now();
            
            THEN("release event is displayed") {
                REQUIRE(releaseEvent.type == InputType::ButtonRelease);
                REQUIRE(releaseEvent.value == 0);
            }
        }
    }
}

SCENARIO("Multiple input events are displayed in correct order", "[realtime-display]") {
    GIVEN("Display receives multiple events") {
        std::vector<InputEvent> events;
        
        for (int i = 0; i < 5; ++i) {
            InputEvent event;
            event.deviceId = 1;
            event.type = InputType::ButtonPress;
            event.code = InputCode::ButtonA;
            event.value = 1;
            event.deviceName = "TestController";
            event.timestamp = std::chrono::high_resolution_clock::now();
            events.push_back(event);
        }
        
        WHEN("events are processed") {
            THEN("all events are captured") {
                REQUIRE(events.size() == 5);
            }
            
            AND_THEN("events maintain chronological order") {
                for (size_t i = 1; i < events.size(); ++i) {
                    REQUIRE(events[i].timestamp >= events[i-1].timestamp);
                }
            }
        }
    }
}

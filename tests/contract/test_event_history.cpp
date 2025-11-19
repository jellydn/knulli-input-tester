#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include "../../src/input/event_history.h"

using namespace knulli::input;

SCENARIO("Event history stores and retrieves events correctly", "[event-history]") {
    GIVEN("Empty event history") {
        EventHistory history;
        
        WHEN("events are added to history") {
            for (int i = 0; i < 10; ++i) {
                InputEvent event;
                event.deviceId = i % 2;  // Alternate between 2 devices
                event.type = InputType::ButtonPress;
                event.code = InputCode::ButtonA;
                event.value = 1;
                event.deviceName = "TestDevice";
                history.addEvent(event);
            }
            
            THEN("all events are stored") {
                REQUIRE(history.size() == 10);
                REQUIRE(!history.isEmpty());
            }
            
            AND_THEN("events can be retrieved") {
                auto events = history.getEvents(10);
                REQUIRE(events.size() == 10);
            }
        }
    }
}

SCENARIO("Event history maintains max size limit", "[event-history]") {
    GIVEN("Event history with max size limit") {
        EventHistory history;
        size_t maxSize = history.MAX_EVENTS;
        
        WHEN("more events than max are added") {
            for (size_t i = 0; i < maxSize + 100; ++i) {
                InputEvent event;
                event.deviceId = 1;
                event.type = InputType::ButtonPress;
                event.code = InputCode::ButtonA;
                history.addEvent(event);
            }
            
            THEN("history never exceeds max size") {
                REQUIRE(history.size() == maxSize);
                REQUIRE(history.size() <= history.MAX_EVENTS);
            }
            
            AND_THEN("oldest events are removed") {
                auto events = history.getEvents(10);
                REQUIRE(events.size() == 10);
            }
        }
    }
}

SCENARIO("Events can be filtered by device ID", "[event-history]") {
    GIVEN("History with events from multiple devices") {
        EventHistory history;
        
        // Add events from device 1
        for (int i = 0; i < 5; ++i) {
            InputEvent event;
            event.deviceId = 1;
            event.code = InputCode::ButtonA;
            history.addEvent(event);
        }
        
        // Add events from device 2
        for (int i = 0; i < 5; ++i) {
            InputEvent event;
            event.deviceId = 2;
            event.code = InputCode::ButtonB;
            history.addEvent(event);
        }
        
        WHEN("events are filtered by device ID") {
            auto device1Events = history.getEventsForDevice(1);
            auto device2Events = history.getEventsForDevice(2);
            
            THEN("only events from selected device are returned") {
                REQUIRE(device1Events.size() == 5);
                REQUIRE(device2Events.size() == 5);
                
                for (const auto& event : device1Events) {
                    REQUIRE(event.deviceId == 1);
                }
                
                for (const auto& event : device2Events) {
                    REQUIRE(event.deviceId == 2);
                }
            }
        }
    }
}

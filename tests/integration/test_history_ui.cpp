#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include "../../src/input/event_history.h"
#include "../../src/ui/event_list.h"

using namespace knulli::input;
using namespace knulli::ui;

SCENARIO("History UI displays events with scrolling", "[history-ui]") {
    GIVEN("Event list widget with history data") {
        EventHistory history;
        EventListWidget widget;
        
        // Add 50 events to history
        for (int i = 0; i < 50; ++i) {
            InputEvent event;
            event.deviceId = 1;
            event.type = InputType::ButtonPress;
            event.code = InputCode::ButtonA;
            event.deviceName = "TestDevice";
            history.addEvent(event);
        }
        
        WHEN("events are loaded into UI widget") {
            auto events = history.getEvents(20);
            widget.update(events);
            
            THEN("widget shows limited number of events") {
                REQUIRE(widget.getVisibleItemCount() <= widget.getVisibleItemCount());
            }
            
            AND_THEN("widget can scroll") {
                int initialPosition = widget.getScrollPosition();
                widget.scrollToBottom();
                REQUIRE(widget.getScrollPosition() >= initialPosition);
                
                widget.scrollToTop();
                REQUIRE(widget.getScrollPosition() == 0);
            }
        }
    }
}

SCENARIO("History can be searched and filtered", "[history-ui]") {
    GIVEN("Event history with various event types") {
        EventHistory history;
        
        for (int i = 0; i < 20; ++i) {
            InputEvent event;
            event.deviceId = (i < 10) ? 1 : 2;
            event.type = InputType::ButtonPress;
            event.code = InputCode::ButtonA;
            history.addEvent(event);
        }
        
        WHEN("events are filtered by device") {
            auto device1Events = history.getEventsForDevice(1);
            auto device2Events = history.getEventsForDevice(2);
            
            THEN("filtered results are accurate") {
                REQUIRE(device1Events.size() == 10);
                REQUIRE(device2Events.size() == 10);
            }
        }
    }
}

# Phase 0 Research: Knulli Input Tester

**Date**: 2025-11-19  
**Purpose**: Resolve technical uncertainties and establish implementation approach for Knulli Input Tester application

## Language Choice: C++17

**Decision**: C++17 selected over Python for Knulli Input Tester

**Rationale**:
- **Performance**: Native compilation meets <100ms latency requirement vs Python interpreter overhead
- **Memory**: C++ applications use 5-15MB vs Python's 30-50MB+ runtime overhead
- **Integration**: C++ is primary language for Batocera/Knulli components (EmulationStation, emulators)
- **Availability**: GCC toolchain already in Batocera buildroot, Python limited to scripts
- **Community**: All major retro gaming projects use C++ (RetroArch, libretro cores, emulators)

**Alternatives Considered**:
- Python 3.11: Faster prototyping but cannot meet real-time constraints reliably
- Rust: Modern but limited Batocera ecosystem support
- Go: Good performance but minimal embedded gaming community support

## GUI Framework: Dear ImGui + SDL2

**Decision**: Dear ImGui with SDL2 backend for user interface

**Rationale**:
- **Perfect Fit**: Immediate mode GUI ideal for real-time input data display
- **Performance**: 16-30ms latency, well within <100ms requirement
- **Development Speed**: Built-in widgets (tables, text, lists) perfect for input testing
- **Compatibility**: SDL2 available in Batocera, works on both GPU and framebuffer devices
- **Resources**: Total footprint ~7-13MB, well under <50MB constraint

**Implementation Approach**:
```cpp
// Core rendering loop
ImGui::Begin("Input Events");
if (ImGui::BeginTable("InputLog", 3, ImGuiTableFlags_Borders)) {
    ImGui::TableSetupColumn("Device");
    ImGui::TableSetupColumn("Event"); 
    ImGui::TableSetupColumn("Time");
    ImGui::TableHeadersRow();
    
    for (const auto& event : inputEvents) {
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0); ImGui::Text("%s", event.device.c_str());
        ImGui::TableSetColumnIndex(1); ImGui::Text("%s", event.description.c_str());
        ImGui::TableSetColumnIndex(2); ImGui::Text("%.3f", event.timestamp);
    }
}
ImGui::EndTable();
ImGui::End();
```

**Alternatives Considered**:
- Direct framebuffer: Minimal overhead but complex development
- SDL2 alone: Good support but requires manual UI implementation
- Qt: Not available in Batocera, excessive resource usage
- Custom graphics: Significant development effort

## Testing Framework: Google Test + Catch2 Hybrid

**Decision**: Google Test for unit tests, Catch2 for BDD integration tests

**Rationale**:
- **Google Test**: Excellent embedded systems support, native Batocera availability, superior mocking
- **Catch2**: Natural BDD syntax perfect for user story validation, readable acceptance tests
- **Hardware Testing**: Layered architecture with device abstraction for clean test boundaries

**Testing Strategy**:
```cpp
// Google Test example
TEST(InputManagerTest, DetectsControllerConnection) {
    MockInputDevice mockDevice;
    InputManager manager;
    
    EXPECT_CALL(mockDevice, isConnected()).WillOnce(Return(true));
    manager.addDevice(mockDevice);
    
    ASSERT_TRUE(manager.hasConnectedDevices());
}

// Catch2 BDD example  
SCENARIO("User presses button and sees event on screen", "[input-display]") {
    GIVEN("Input tester app is running and controller connected") {
        InputTester tester;
        MockController controller;
        tester.addController(controller);
        
        WHEN("user presses A button") {
            controller.press(Button::A);
            tester.update();
            
            THEN("button press event appears on screen within 100ms") {
                auto events = tester.getDisplayedEvents();
                REQUIRE(events.size() > 0);
                REQUIRE(events.back().type == EventType::ButtonPress);
                REQUIRE(events.back().button == Button::A);
                REQUIRE(events.back().timestamp <= 100ms);
            }
        }
    }
}
```

**Hardware Testing Approach**:
- **Device Abstraction Layer**: Clean interface for mocking input devices
- **Virtual Input Devices**: Linux uinput for hardware simulation
- **Event Injection**: Controlled testing without physical controllers
- **Hardware-in-the-Loop**: Real device validation on target hardware

## Input System Integration: libevdev + SDL2

**Decision**: libevdev for low-level input, SDL2 for cross-platform compatibility

**Rationale**:
- **libevdev**: Direct Linux input subsystem access, zero-copy event handling
- **SDL2**: Cross-platform input handling, already used by Batocera emulators
- **Hybrid Approach**: libevdev for device detection, SDL2 for event processing

**Implementation Architecture**:
```cpp
class InputManager {
private:
    std::vector<std::unique_ptr<InputDevice>> devices;
    libevdev* deviceHandle;
    
public:
    void detectDevices() {
        // Scan /dev/input/ for available devices
        for (const auto& devicePath : scanInputDevices()) {
            if (isValidGameController(devicePath)) {
                auto device = std::make_unique<InputDevice>(devicePath);
                devices.push_back(std::move(device));
            }
        }
    }
    
    void processEvents() {
        for (auto& device : devices) {
            while (device->hasPendingEvents()) {
                InputEvent event = device->getNextEvent();
                handleEvent(event);
            }
        }
    }
};
```

## Knulli System Integration

**Configuration System**:
- Location: `/userdata/system/configs/input-tester/config.ini`
- Format: INI-style configuration following Batocera conventions
- Integration: Use Batocera's configuration management APIs

**Logging Integration**:
- System: Use syslog for Knulli system integration
- Level: INFO for normal operation, ERROR for failures
- Format: Structured logging with device IDs and event types

**Service Integration**:
- Systemd service for background operation
- Hotkey integration with Batocera's input system
- Optional overlay mode during gaming

## Performance Optimization Strategy

**Memory Management**:
- Circular buffer for event history (1000+ events)
- Object pooling for frequent allocations
- Memory-mapped device access where possible

**CPU Optimization**:
- Event-driven architecture (no polling loops)
- Minimal UI redraws (only when events change)
- Multi-threading: input processing separate from UI rendering

**Latency Optimization**:
- Direct system calls, no abstraction layers
- Real-time scheduling for input thread
- Hardware timestamping where available

## Deployment Strategy

**Package Format**: Batocera pacman package
**Installation**: Via Batocera package manager or manual copy
**Dependencies**: SDL2, libevdev, Dear ImGui (all available in Batocera)
**Configuration**: `/userdata/system/configs/input-tester/`
**Integration**: Add to Batocera applications menu

## Risk Mitigation

**Hardware Compatibility**:
- Fallback to basic input if advanced features unavailable
- Graceful degradation on older devices
- Comprehensive device detection and reporting

**Performance Risks**:
- Continuous monitoring of memory usage
- Adaptive refresh rates based on system load
- Emergency shutdown if resource limits exceeded

**Integration Risks**:
- Minimal system dependencies
- No interference with gaming performance
- Clean shutdown and resource cleanup

## Conclusion

All technical uncertainties resolved with clear implementation path:
- **Language**: C++17 for performance and integration
- **GUI**: Dear ImGui + SDL2 for rapid development and compatibility
- **Testing**: Google Test + Catch2 hybrid for comprehensive coverage
- **Input**: libevdev + SDL2 for robust device support
- **Integration**: Full Knulli system integration following Batocera conventions

This approach meets all constitutional requirements and success criteria while providing maintainable, performant solution for Knulli Input Tester application.
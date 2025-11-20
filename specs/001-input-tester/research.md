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

## Cross-Compilation Strategy for TrimUI Smart Pro

**CRITICAL UPDATE (2025-11-20)**: PR review revealed architecture mismatch - application must be built for Linux ARM, not macOS.

**Target Architecture**:
- **Device**: TrimUI Smart Pro with Allwinner A133P
- **Processor**: ARM Cortex-A53, 32-bit ARMv7 or 64-bit ARMv8-A
- **OS**: Knulli Linux (based on Batocera)
- **Binary Format**: ELF (Linux), not Mach-O (macOS)

**Build Environment**:
- Compile on Linux x86_64 or macOS using ARM cross-compiler
- **Toolchain Required**: 
  - `aarch64-linux-gnu-gcc` for 64-bit ARM
  - `arm-linux-gnueabihf-gcc` for 32-bit ARM
- CMake cross-compilation file for target specification
- Static linking for SDL2, Dear ImGui, libevdev to ensure compatibility

**Dependency Verification**:
- **SDL2**: Not guaranteed on Knulli - must static link or provide
- **Dear ImGui**: Not part of standard Knulli - must include or static link
- **libevdev**: Likely available but verify version compatibility
- **Build Strategy**: Static linking for maximum compatibility across Knulli variants

**CMake Configuration**:
```cmake
# Cross-compilation toolchain
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR aarch64)
set(CMAKE_C_COMPILER aarch64-linux-gnu-gcc)
set(CMAKE_CXX_COMPILER aarch64-linux-gnu-g++)

# Static linking for portability
set(BUILD_SHARED_LIBS OFF)
set(SDL2_STATIC ON)
```

## PortMaster Integration for Knulli Distribution

**Application Distribution**: PortMaster is the standard package manager for Knulli.

**Installation Path**:
- App location: `/roms/ports/knulli-input-tester/`
- Launcher script: `/roms/ports/knulli-input-tester/knulli-input-tester.sh`
- Support files: Config, docs, resources in same directory

**Launcher Script Requirements**:
```bash
#!/bin/bash
# PortMaster compatible launcher
# Must handle display, switch to app directory, run binary, restore display
```

**Configuration Integration**:
- Store config in: `/roms/ports/knulli-input-tester/config.ini`
- Create config on first run with defaults
- Support per-user config overrides

## Display & EmulationStation Integration

**Critical Concern**: EmulationStation owns exclusive display access on Knulli.

**Display Handling Strategies**:
1. **Fullscreen Mode** (preferred):
   - SDL2 fullscreen exclusive access
   - May need to pause/resume EmulationStation
   - PortMaster launcher typically handles this

2. **Alternative Framebuffer Access**:
   - Direct `/dev/fb0` framebuffer write if SDL2 unavailable
   - Requires permission setup in PortMaster
   - Less flexible but works on all Knulli devices

3. **Graceful Degradation**:
   - If display unavailable, run headless mode
   - Log events to file for review
   - Report clear error messages

## Deployment Strategy

**Package Format**: PortMaster package for Knulli distribution
**Installation**: Via PortMaster package manager on Knulli device
**Build Target**: Linux ARM64 (aarch64) ELF binary via cross-compilation
**Dependencies**: Statically linked SDL2, Dear ImGui; runtime libevdev check
**Configuration**: `/roms/ports/knulli-input-tester/config.ini`
**Integration**: PortMaster launcher script with EmulationStation display coordination

## Risk Mitigation

**Architecture & Cross-Compilation Risks** 🔴 CRITICAL:
- **Risk**: Binary compiled for macOS cannot run on Linux ARM device
- **Mitigation**: 
  - Implement proper cross-compilation toolchain in CMakeLists.txt
  - Create cross-compilation guide with aarch64-linux-gnu-gcc setup
  - Automated target architecture verification in build system
  - CI/CD pipeline to build for target architecture before deployment

**Dependency Availability Risks** 🟡 HIGH:
- **Risk**: SDL2 and Dear ImGui may not be available on all Knulli variants
- **Mitigation**:
  - Static link all GUI dependencies for portability
  - Provide dependency bundle in PortMaster package
  - Implement fallback to framebuffer access if SDL2 unavailable
  - Automated dependency verification at startup

**Display Access Conflicts** 🟡 HIGH:
- **Risk**: EmulationStation owns exclusive display, may prevent app display
- **Mitigation**:
  - PortMaster launcher script handles display pause/resume
  - Test on actual TrimUI Smart Pro hardware
  - Implement headless mode for display-less operation
  - Clear user messaging if display unavailable

**Hardware Compatibility**:
- Fallback to basic input if advanced features unavailable
- Graceful degradation on older devices
- Comprehensive device detection and reporting
- Test across multiple Knulli device variants

**Performance Risks**:
- Continuous monitoring of memory usage
- Adaptive refresh rates based on system load
- Emergency shutdown if resource limits exceeded

**Integration Risks**:
- Minimal system dependencies
- No interference with gaming performance
- Clean shutdown and resource cleanup
- PortMaster compatibility testing

## Conclusion

Technical approach refined based on PR review findings:

**Core Architecture**:
- **Language**: C++17 for performance and integration
- **GUI**: Dear ImGui + SDL2 for rapid development and compatibility
- **Testing**: Google Test + Catch2 hybrid for comprehensive coverage
- **Input**: libevdev + SDL2 for robust device support

**Critical Updates Required**:
- **Cross-Compilation**: Build system must target Linux ARM64 (aarch64) via cross-compiler, NOT macOS native
- **Deployment**: PortMaster integration for standard Knulli distribution (not direct installation)
- **Dependencies**: Static linking strategy for SDL2 and Dear ImGui to ensure compatibility
- **Display**: PortMaster launcher script handling with EmulationStation coordination

**Build Artifacts**:
- Primary: ELF format Linux ARM64 binary (aarch64-linux-gnu)
- Alternative: ARM32 (armeabihf) support for older devices
- Package: PortMaster-compatible structure with launcher script

**Testing Strategy Updated**:
- Unit/integration tests: Run on development machine (existing)
- Hardware validation: MUST test on actual TrimUI Smart Pro with Knulli OS
- Cross-compilation verification: Verify binary format before deployment
- PortMaster integration: Test launcher script and display handling

This approach meets all constitutional requirements while addressing critical deployment constraints for TrimUI Smart Pro with Knulli OS. Architecture targeting and cross-compilation are non-negotiable requirements before hardware deployment.
# Testing Framework Research: Knulli Input Tester Application

**Date**: 2025-11-19  
**Target**: C++ Input Testing Application for Knulli Linux Embedded Gaming Handhelds  
**Dependencies**: SDL2 + Dear ImGui + libevdev

## Executive Summary

After comprehensive research of testing frameworks and embedded systems testing approaches, I recommend a **hybrid testing strategy** combining:

1. **Google Test** as the primary unit testing framework
2. **Catch2** for BDD-style integration tests  
3. **Custom hardware mocking** for input device simulation
4. **Linux kselftest** integration for kernel-level validation
5. **Hardware-in-the-loop testing** for real device validation

This approach provides the best balance of Batocera/Knulli compatibility, hardware dependency management, and comprehensive testing coverage.

## 1. Unit Testing Framework Comparison

### Google Test (Recommended Primary Framework)

**Pros:**
- **Excellent embedded systems support** - widely used in Android, ChromeOS, and embedded Linux
- **Batocera availability** - already included in Buildroot package system
- **Comprehensive mocking framework** (Google Mock) for hardware abstraction
- **Mature ecosystem** - 37.5k stars, extensive documentation
- **Cross-platform compatibility** - works on ARM64/x86_64 Knulli targets
- **CI/CD integration** - excellent GitHub Actions and Jenkins support

**Cons:**
- More verbose syntax compared to Catch2
- Requires separate Google Mock dependency

**Batocera Integration:**
```bash
# Available in Buildroot as package
BR2_PACKAGE_GTEST=y
BR2_PACKAGE_GMOCK=y
```

### Catch2 (Recommended for BDD Tests)

**Pros:**
- **Simple, natural syntax** - very readable test descriptions
- **Header-only (v2.x)** or library (v3.x) - flexible deployment
- **Built-in BDD support** - excellent for user story testing
- **Excellent error messages** - detailed failure reporting

**Cons:**
- Less mature mocking ecosystem compared to Google Test
- v3.x requires C++14+ (good for your C++17 requirement)

**Usage Example:**
```cpp
SCENARIO("Controller input is processed correctly", "[input][controller]") {
    GIVEN("A connected Xbox controller") {
        MockInputDevice device("Xbox Controller");
        InputProcessor processor;
        
        WHEN("A button is pressed") {
            device.simulateButtonPress(XBOX_A);
            auto events = processor.processEvents();
            
            THEN("The event appears within 100ms") {
                REQUIRE(events.size() == 1);
                REQUIRE(events[0].type == BUTTON_PRESS);
                REQUIRE(events[0].timestamp <= now() + 100ms);
            }
        }
    }
}
```

### Framework Decision Matrix

| Criteria | Google Test | Catch2 | Recommendation |
|-----------|-------------|---------|----------------|
| Batocera Availability | ✅ Native | ✅ Available | Google Test |
| Mocking Support | ✅ Excellent | ⚠️ Limited | Google Test |
| BDD Support | ⚠️ Basic | ✅ Excellent | Catch2 |
| Learning Curve | ⚠️ Moderate | ✅ Easy | Catch2 |
| Embedded Support | ✅ Excellent | ✅ Good | Google Test |

## 2. Hardware-Dependent Testing Strategies

### Mock/Fake Strategy for Input Devices

**Recommended Approach: Layered Mocking Architecture**

```
┌─────────────────┐
│   Application   │
├─────────────────┤
│  Input Manager  │ ← Test Boundary
├─────────────────┤
│ Device Abstraction Layer (DAL)
├─────────────────┤
│ libevdev/SDL2   │ ← Mock Point
└─────────────────┘
```

**Mock Implementation Strategy:**

1. **Device Abstraction Layer (DAL)**
```cpp
// Abstract interface for all input devices
class IInputDevice {
public:
    virtual ~IInputDevice() = default;
    virtual std::vector<InputEvent> pollEvents() = 0;
    virtual bool isConnected() const = 0;
    virtual std::string getDeviceName() const = 0;
    virtual DeviceType getDeviceType() const = 0;
};

// Real implementation using libevdev
class EvdevDevice : public IInputDevice {
    libevdev* dev_;
    // Real hardware implementation
};

// Mock implementation for testing
class MockInputDevice : public IInputDevice {
    std::queue<InputEvent> event_queue_;
    bool connected_;
    // Test implementation
};
```

2. **Event Injection System**
```cpp
class TestEventInjector {
public:
    void simulateButtonPress(ButtonCode button, Timestamp timestamp = now());
    void simulateButtonRelease(ButtonCode button, Timestamp timestamp = now());
    void simulateAxisMove(AxisCode axis, int value, Timestamp timestamp = now());
    void simulateDeviceConnection(const std::string& device_name);
    void simulateDeviceDisconnection();
};
```

3. **Device Factory Pattern**
```cpp
enum class TestMode { REAL, MOCK, SIMULATION };

class InputDeviceFactory {
public:
    static std::unique_ptr<IInputDevice> createDevice(
        const std::string& path, 
        TestMode mode = TestMode::REAL
    );
};
```

### Integration Testing Approaches

**1. Virtual Input Device Testing (uinput)**
```cpp
// Create virtual input devices for testing
class VirtualInputDevice {
    int uinput_fd_;
    
public:
    void createVirtualController(const std::string& name);
    void injectEvent(int type, int code, int value);
    void destroyDevice();
};
```

**2. SDL2 Mock Subsystem**
```cpp
// Mock SDL2 input subsystem
class MockSDLSubsystem {
    static bool initialized_;
    static std::vector<SDL_Event> event_queue_;
    
public:
    static void pushEvent(const SDL_Event& event);
    static SDL_Event pollEvent();
    static void setJoystickCount(int count);
};
```

## 3. Automated Testing on Embedded Systems

### Linux kselftest Integration

**Why kselftest?**
- **Native kernel testing** - validates input subsystem directly
- **Batocera integration** - already part of kernel build system
- **Hardware validation** - tests real input drivers
- **CI/CD ready** - can be automated in build pipelines

**Implementation:**
```makefile
# tests/kselftest/input/Makefile
TEST_GEN_PROGS := input-tester-kselftest
TEST_GEN_FILES := test-input-events.sh

CFLAGS += -I../../../include -I../../../usr/include
LDLIBS += -levdev

include ../../lib.mk
```

**Test Structure:**
```c
// tests/kselftest/input/input-tester-kselftest.c
#include "kselftest.h"

TEST(test_device_detection) {
    // Test input device detection
    int fd = open("/dev/input/event0", O_RDONLY);
    ASSERT_GE(fd, 0);
    close(fd);
}

TEST(test_event_processing) {
    // Test event processing latency
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);
    
    // Simulate input event
    // ... event injection code ...
    
    clock_gettime(CLOCK_MONOTONIC, &end);
    long latency_ms = (end.tv_sec - start.tv_sec) * 1000 + 
                     (end.tv_nsec - start.tv_nsec) / 1000000;
    
    ASSERT_LT(latency_ms, 100); // < 100ms requirement
}

TEST_HARNESS_MAIN
```

### Cross-Compilation Testing

**Buildroot Integration:**
```makefile
# package/input-tester/Config.in
config BR2_PACKAGE_INPUT_TESTER
    bool "input-tester"
    depends on BR2_PACKAGE_SDL2
    depends on BR2_PACKAGE_IMGUI
    help
      Knulli Input Tester application

if BR2_PACKAGE_INPUT_TESTER

config BR2_PACKAGE_INPUT_TESTER_TESTS
    bool "Build tests"
    select BR2_PACKAGE_GTEST
    select BR2_PACKAGE_GMOCK
    help
      Build unit and integration tests

endif
```

## 4. CI/CD Considerations for Batocera/Knulli

### GitHub Actions Workflow

```yaml
name: Input Tester CI

on: [push, pull_request]

jobs:
  build-and-test:
    runs-on: ubuntu-latest
    container: batocera/batocera-buildroot:latest
    
    steps:
    - uses: actions/checkout@v3
    
    - name: Configure Buildroot
      run: |
        make batocera-rk3588_defconfig
        echo "BR2_PACKAGE_INPUT_TESTER=y" >> .config
        echo "BR2_PACKAGE_INPUT_TESTER_TESTS=y" >> .config
    
    - name: Build Application
      run: |
        make input-tester
    
    - name: Run Unit Tests
      run: |
        make input-tester-test
    
    - name: Run Hardware Tests
      run: |
        # Setup virtual input devices
        sudo modprobe uinput
        sudo chmod 666 /dev/uinput
        
        # Run hardware-in-the-loop tests
        ./output/target/usr/bin/input-tester-hwtest
    
    - name: Generate Test Report
      run: |
        ./output/target/usr/bin/input-tester-coverage
```

### Hardware-in-the-Loop Testing

**1. Device Farm Integration**
```python
# scripts/hardware_test_runner.py
class HardwareTestRunner:
    def __init__(self, device_configs):
        self.devices = self.connect_devices(device_configs)
    
    def run_test_suite(self, test_binary):
        results = {}
        for device in self.devices:
            # Deploy test to device
            device.deploy(test_binary)
            
            # Run tests with real hardware
            result = device.run_tests()
            results[device.id] = result
            
        return results
```

**2. Automated Controller Testing**
```cpp
// Hardware test automation
class ControllerTestAutomation {
public:
    struct TestScenario {
        std::string name;
        std::vector<ButtonPress> button_sequence;
        std::chrono::milliseconds max_latency;
    };
    
    bool runScenario(const TestScenario& scenario) {
        // Connect test controller
        auto controller = connectTestController();
        
        // Execute test sequence
        auto start = std::chrono::high_resolution_clock::now();
        for (const auto& press : scenario.button_sequence) {
            controller.pressButton(press.button, press.duration);
        }
        
        // Verify response
        return verifyEvents(scenario.max_latency);
    }
};
```

## 5. Performance Testing Capabilities

### Latency Measurement Framework

```cpp
class LatencyTester {
public:
    struct LatencyResult {
        std::chrono::microseconds min_latency;
        std::chrono::microseconds max_latency;
        std::chrono::microseconds avg_latency;
        std::chrono::microseconds p95_latency;
        size_t sample_count;
    };
    
    LatencyResult measureInputLatency(
        std::chrono::seconds test_duration,
        std::chrono::milliseconds target_latency = 100ms
    ) {
        std::vector<std::chrono::microseconds> latencies;
        
        auto start_time = std::chrono::steady_clock::now();
        while (std::chrono::steady_clock::now() - start_time < test_duration) {
            auto input_time = injectTestEvent();
            auto response_time = waitForResponse();
            
            latencies.push_back(response_time - input_time);
        }
        
        return calculateStatistics(latencies);
    }
};
```

### Stress Testing Framework

```cpp
class InputStressTester {
public:
    void runStressTest(const StressTestConfig& config) {
        std::vector<std::thread> threads;
        
        // Spawn multiple input threads
        for (int i = 0; i < config.concurrent_inputs; ++i) {
            threads.emplace_back([this, &config, i]() {
                for (int j = 0; j < config.events_per_thread; ++j) {
                    injectRandomEvent(i * 1000 + j);
                    std::this_thread::sleep_for(
                        std::chrono::microseconds(config.event_interval_us)
                    );
                }
            });
        }
        
        // Monitor system performance
        auto monitor_thread = std::thread([&]() {
            monitorSystemResources(config.test_duration);
        });
        
        // Wait for completion
        for (auto& t : threads) t.join();
        monitor_thread.join();
    }
};
```

## 6. Specific Recommendations for Input Tester Application

### Recommended Testing Architecture

```
┌─────────────────────────────────────────────────────────┐
│                    CI/CD Pipeline                      │
├─────────────────────────────────────────────────────────┤
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐   │
│  │   Unit      │  │Integration  │  │   Hardware  │   │
│  │   Tests     │  │   Tests     │  │   Tests     │   │
│  │ (Google     │  │ (Catch2     │  │ (kselftest  │   │
│  │  Test)      │  │  BDD)       │  │ + Custom)   │   │
│  └─────────────┘  └─────────────┘  └─────────────┘   │
└─────────────────────────────────────────────────────────┘
```

### Implementation Priority

**Phase 1: Foundation (Week 1-2)**
1. Set up Google Test framework in Buildroot
2. Create Device Abstraction Layer
3. Implement basic mock input devices
4. Set up CI/CD pipeline

**Phase 2: Core Testing (Week 3-4)**
1. Unit tests for input processing logic
2. Mock-based integration tests
3. Performance measurement framework
4. Basic hardware validation

**Phase 3: Advanced Testing (Week 5-6)**
1. Catch2 BDD tests for user stories
2. kselftest integration
3. Hardware-in-the-loop automation
4. Stress testing and edge case validation

### Testing Coverage Goals

| Component | Unit Test Coverage | Integration Test Coverage | Hardware Test Coverage |
|-----------|-------------------|--------------------------|------------------------|
| Input Device Manager | 95% | 90% | 80% |
| Event Processing | 98% | 95% | 85% |
| UI Display | 90% | 85% | 70% |
| System Integration | 85% | 90% | 95% |

### Hardware Dependency Solutions

**1. Development Environment**
```bash
# Setup virtual input devices for development
sudo modprobe uinput
python3 scripts/setup_virtual_devices.py

# Run tests with virtual hardware
./build/tests/input_tester --use_virtual_devices
```

**2. CI Environment**
```yaml
# GitHub Actions with virtual hardware
- name: Setup Virtual Input
  run: |
    sudo modprobe uinput
    docker run --privileged -v /dev:/dev input-tester-env
    
- name: Run Hardware Tests
  run: |
    ./tests/run_hardware_tests.sh --virtual
```

**3. Production Testing**
```bash
# Deploy to actual Knulli device
scp input-tester root@knulli-device:/usr/bin/
ssh root@knulli-device "input-tester --test-mode --real-hardware"
```

## 7. Conclusion

The recommended hybrid testing approach provides:

✅ **Comprehensive Coverage** - Unit, integration, and hardware testing  
✅ **Batocera Compatibility** - All frameworks available in Buildroot  
✅ **Hardware Independence** - Mock-based development with real hardware validation  
✅ **Performance Validation** - Latency and stress testing capabilities  
✅ **CI/CD Ready** - Automated testing pipeline integration  
✅ **Scalable Architecture** - Easy to extend for new input devices  

This strategy ensures the Input Tester application meets all requirements for real-time input processing (<100ms latency), multi-controller support, and reliable operation on Knulli Linux embedded gaming handhelds.

The combination of Google Test's robust mocking framework with Catch2's readable BDD syntax provides the best of both worlds for comprehensive testing coverage while maintaining compatibility with the Batocera/Knulli development ecosystem.
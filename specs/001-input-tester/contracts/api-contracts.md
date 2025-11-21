# API Contracts: Knulli Input Tester

**Date**: 2025-11-19  
**Purpose**: Define interfaces and contracts for input testing application components

## Core System Interfaces

### Input Device Manager Interface

```cpp
class IInputDeviceManager {
public:
    virtual ~IInputDeviceManager() = default;
    
    // Device lifecycle management
    virtual std::vector<std::shared_ptr<InputDevice>> getConnectedDevices() const = 0;
    virtual std::shared_ptr<InputDevice> getDevice(int deviceId) const = 0;
    virtual bool addDevice(const std::string& devicePath) = 0;
    virtual void removeDevice(int deviceId) = 0;
    
    // Event processing
    virtual std::vector<InputEvent> processPendingEvents() = 0;
    virtual bool hasPendingEvents() const = 0;
    
    // Device monitoring
    virtual void startDeviceMonitoring() = 0;
    virtual void stopDeviceMonitoring() = 0;
    
    // Events
    std::function<void(int deviceId)> onDeviceConnected;
    std::function<void(int deviceId)> onDeviceDisconnected;
    std::function<void(const InputEvent&)> onInputEvent;
};
```

### Input Device Interface

```cpp
class IInputDevice {
public:
    virtual ~IInputDevice() = default;
    
    // Device information
    virtual int getDeviceId() const = 0;
    virtual std::string getName() const = 0;
    virtual std::string getUniqueId() const = 0;
    virtual ConnectionType getConnectionType() const = 0;
    virtual DeviceCapabilities getCapabilities() const = 0;
    virtual DeviceState getState() const = 0;
    
    // Event handling
    virtual bool hasPendingEvents() const = 0;
    virtual std::optional<InputEvent> getNextEvent() = 0;
    virtual std::vector<InputEvent> getAllPendingEvents() = 0;
    
    // Device control
    virtual bool connect() = 0;
    virtual void disconnect() = 0;
    virtual bool isConnected() const = 0;
    
    // Configuration
    virtual void setButtonMapping(ButtonScheme scheme) = 0;
    virtual ButtonScheme getButtonMapping() const = 0;
};
```

### Event History Interface

```cpp
class IEventHistory {
public:
    virtual ~IEventHistory() = default;
    
    // Event storage
    virtual void addEvent(const InputEvent& event) = 0;
    virtual void addEvents(const std::vector<InputEvent>& events) = 0;
    virtual void clear() = 0;
    
    // Event retrieval
    virtual std::vector<InputEvent> getEvents(size_t count = MAX_EVENTS) const = 0;
    virtual std::vector<InputEvent> getEventsForDevice(int deviceId, size_t count = MAX_EVENTS) const = 0;
    virtual std::vector<InputEvent> getEventsInTimeRange(
        std::chrono::milliseconds start,
        std::chrono::milliseconds end
    ) const = 0;
    
    // History management
    virtual size_t size() const = 0;
    virtual bool isEmpty() const = 0;
    virtual std::chrono::milliseconds getOldestEventTime() const = 0;
    virtual std::chrono::milliseconds getNewestEventTime() const = 0;
    
    // Filtering and search
    virtual std::vector<InputEvent> getEventsOfType(InputType type) const = 0;
    virtual std::vector<InputEvent> getEventsWithCode(InputCode code) const = 0;
    virtual size_t getEventCount(int deviceId, InputType type) const = 0;
};
```

### Display Interface

```cpp
class IDisplay {
public:
    virtual ~IDisplay() = default;
    
    // Display lifecycle
    virtual bool initialize() = 0;
    virtual void shutdown() = 0;
    virtual bool isInitialized() const = 0;
    
    // Rendering
    virtual void beginFrame() = 0;
    virtual void endFrame() = 0;
    virtual void clear() = 0;
    virtual void present() = 0;
    
    // UI elements
    virtual void drawEventList(const std::vector<InputEvent>& events) = 0;
    virtual void drawDeviceStatus(const std::vector<std::shared_ptr<InputDevice>>& devices) = 0;
    virtual void drawSystemInfo(const SystemInfo& info) = 0;
    
    // Configuration
    virtual void setRefreshRate(int fps) = 0;
    virtual int getRefreshRate() const = 0;
    virtual void setDisplayMode(DisplayMode mode) = 0;
    
    // Events
    std::function<void()> onRequestClose;
    std::function<void(DisplayMode)> onModeChange;
};

enum class DisplayMode {
    Windowed,       // Standalone window
    Overlay,         // Overlay on top of games
    Fullscreen       // Fullscreen exclusive
};
```

## Configuration Contracts

### Application Configuration

```ini
[input-tester]
# Display settings
display_mode=windowed
refresh_rate=60
max_history_events=1000
show_timestamps=true
show_device_names=true

# Input settings
auto_detect_devices=true
function_button_mode=hotkey
button_scheme=knulli
ignore_duplicate_events=true
duplicate_threshold_ms=50

# Performance settings
max_cpu_usage=10
memory_limit_mb=50
enable_hardware_acceleration=true

# Logging settings
log_level=info
log_to_file=false
log_to_syslog=true
log_file_path=/userdata/system/logs/input-tester.log

# Integration settings
enable_hotkey=true
hotkey_combo=Function+Select
overlay_enabled=false
auto_start=false
```

### Device Configuration Schema

```json
{
  "device_config": {
    "device_id": "int",
    "name": "string",
    "custom_name": "string",
    "button_scheme": "knulli|playstation|xbox",
    "deadzone": {
      "left_stick": "float[0.0-1.0]",
      "right_stick": "float[0.0-1.0]",
      "triggers": "float[0.0-1.0]"
    },
    "sensitivity": {
      "left_stick": "float[0.1-5.0]",
      "right_stick": "float[0.1-5.0]"
    },
    "rumble": {
      "enabled": "boolean",
      "intensity": "float[0.0-1.0]"
    },
    "led": {
      "enabled": "boolean",
      "color": "#RRGGBB"
    }
  }
}
```

## Event Data Contracts

### Input Event Serialization

```json
{
  "input_event": {
    "timestamp": "int64",
    "device_id": "int",
    "device_name": "string",
    "type": "button_press|button_release|axis_motion|hat_switch|device_connect|device_disconnect",
    "code": "string",
    "value": "int",
    "raw_code": "int",
    "raw_value": "int",
    "mapped_code": "string",
    "description": "string"
  }
}
```

### Device Information

```json
{
  "device_info": {
    "id": "int",
    "name": "string",
    "unique_id": "string",
    "connection_type": "builtin|usb|bluetooth",
    "capabilities": {
      "has_buttons": "boolean",
      "has_axes": "boolean",
      "has_hats": "boolean",
      "button_count": "int",
      "axis_count": "int",
      "hat_count": "int",
      "supports_rumble": "boolean",
      "supports_gyro": "boolean"
    },
    "state": "connected|disconnected|error|initializing",
    "last_activity": "int64"
  }
}
```

## System Integration Contracts

### Knulli Service Integration

```bash
# Systemd service file
[Unit]
Description=Knulli Input Tester
Documentation=https://knulli.org/
After=graphical-session.target
Wants=graphical-session.target

[Service]
Type=simple
ExecStart=/usr/bin/input-tester
Restart=on-failure
RestartSec=5
User=root
Environment=DISPLAY=:0

[Install]
WantedBy=graphical-session.target
```

### Configuration File Locations

```
/userdata/system/configs/input-tester/
├── config.ini              # Main configuration
├── devices/                # Per-device configurations
│   ├── device_001.json
│   ├── device_002.json
│   └── ...
└── profiles/               # Input profiles
    ├── default.json
    ├── retro_gaming.json
    └── testing.json
```

### Logging Format

```json
{
  "log_entry": {
    "timestamp": "string",
    "level": "debug|info|warn|error",
    "component": "string",
    "device_id": "int|null",
    "event_type": "string|null",
    "message": "string",
    "details": {
      "key": "value"
    }
  }
}
```

## Performance Contracts

### Latency Requirements

```cpp
struct PerformanceMetrics {
    // Input processing latency
    std::chrono::microseconds inputToDisplayLatency;  // < 100ms required
    std::chrono::microseconds eventProcessingTime;    // < 10ms target
    std::chrono::microseconds uiRenderTime;          // < 16ms (60fps)
    
    // Memory usage
    size_t currentMemoryUsage;                      // < 50MB required
    size_t peakMemoryUsage;                         // Monitor for leaks
    size_t eventHistorySize;                        // Monitor buffer usage
    
    // System impact
    double cpuUsagePercent;                         // < 10% target
    size_t fileDescriptorCount;                      // Monitor resource usage
};
```

### Resource Limits

```cpp
struct ResourceLimits {
    static constexpr size_t MAX_MEMORY_MB = 50;
    static constexpr double MAX_CPU_PERCENT = 10.0;
    static constexpr size_t MAX_EVENTS = 1000;
    static constexpr size_t MAX_DEVICES = 8;
    static constexpr std::chrono::milliseconds MAX_LATENCY{100};
    static constexpr int MIN_FPS = 30;
    static constexpr int MAX_FPS = 120;
};
```

## Error Handling Contracts

### Error Codes and Messages

```cpp
enum class ErrorCode {
    Success = 0,
    
    // Device errors
    DeviceNotFound = 1000,
    DeviceAccessDenied = 1001,
    DeviceDisconnected = 1002,
    DeviceUnsupported = 1003,
    
    // System errors
    InsufficientMemory = 2000,
    ResourceExhausted = 2001,
    InitializationFailed = 2002,
    
    // Configuration errors
    InvalidConfiguration = 3000,
    ConfigurationNotFound = 3001,
    PermissionDenied = 3002,
    
    // Display errors
    DisplayInitializationFailed = 4000,
    DisplayLost = 4001,
    UnsupportedResolution = 4002
};

struct ErrorInfo {
    ErrorCode code;
    std::string message;
    std::string component;
    std::chrono::system_clock::time_point timestamp;
    std::map<std::string, std::string> context;
};
```

### Error Recovery Strategies

```cpp
class IErrorHandler {
public:
    virtual void handleError(const ErrorInfo& error) = 0;
    virtual bool canRecover(ErrorCode code) = 0;
    virtual bool attemptRecovery(const ErrorInfo& error) = 0;
    virtual void logError(const ErrorInfo& error) = 0;
    
    // Recovery strategies
    virtual void restartComponent(const std::string& component) = 0;
    virtual void fallbackToSafeMode() = 0;
    virtual void requestUserIntervention(const std::string& message) = 0;
};
```

## Testing Contracts

### Mock Interfaces for Testing

```cpp
class MockInputDevice : public IInputDevice {
public:
    // Mock control methods
    void setConnected(bool connected);
    void simulateEvent(const InputEvent& event);
    void setCapabilities(const DeviceCapabilities& caps);
    
    // Test verification
    bool wasConnectCalled() const;
    bool wasDisconnectCalled() const;
    std::vector<InputEvent> getSimulatedEvents() const;
    
private:
    bool connected = false;
    std::vector<InputEvent> simulatedEvents;
    mutable std::mutex mockMutex;
};
```

### Test Data Contracts

```json
{
  "test_scenario": {
    "name": "string",
    "description": "string",
    "devices": [
      {
        "type": "mock|real",
        "configuration": "device_config",
        "events": [
          {
            "timestamp": "int64",
            "type": "string",
            "code": "string",
            "value": "int"
          }
        ]
      }
    ],
    "expected_results": {
      "display_events": "int",
      "history_size": "int",
      "performance_metrics": "performance_metrics"
    }
  }
}
```
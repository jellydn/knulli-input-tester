# Data Model: Knulli Input Tester

**Date**: 2025-11-19  
**Purpose**: Define data structures and entities for input event processing and display

## Core Entities

### InputEvent

Represents a single input action captured from any input device.

```cpp
struct InputEvent {
    std::chrono::high_resolution_clock::time_point timestamp;
    int deviceId;
    InputType type;
    InputCode code;
    int value;
    std::string deviceName;
    std::string description;
};

enum class InputType {
    ButtonPress,
    ButtonRelease,
    AxisMotion,
    HatSwitch,
    DeviceConnect,
    DeviceDisconnect
};

enum class InputCode {
    // Face buttons (KNULLI mapping)
    ButtonA,      // East, ○ on PlayStation
    ButtonB,      // South, ✕ on PlayStation  
    ButtonX,      // North, △ on PlayStation
    ButtonY,      // West, ◻ on PlayStation
    
    // Control buttons
    ButtonStart,
    ButtonSelect,
    ButtonFunction, // KNULLI hotkey button
    
    // D-pad and axes
    DPadUp, DPadDown, DPadLeft, DPadRight,
    AxisLeftX, AxisLeftY, AxisRightX, AxisRightY,
    
    // Triggers and shoulders
    TriggerLeft, TriggerRight,
    ShoulderLeft, ShoulderRight,
    
    // Special
    PowerButton,
    ToggleSwitch,
    Unknown
};
```

### InputDevice

Represents a connected input device with its capabilities and current state.

```cpp
class InputDevice {
public:
    int deviceId;
    std::string name;
    std::string uniqueId;
    ConnectionType connectionType;
    DeviceCapabilities capabilities;
    DeviceState state;
    std::chrono::steady_clock::time_point lastActivity;
    
private:
    libevdev* deviceHandle;
    std::string physicalPath;
};

enum class ConnectionType {
    BuiltIn,       // Device's built-in controls
    USB,           // USB connected controller
    Bluetooth,      // Bluetooth controller
    Unknown
};

struct DeviceCapabilities {
    bool hasButtons;
    bool hasAxes;
    bool hasHats;
    int buttonCount;
    int axisCount;
    int hatCount;
    bool supportsRumble;
    bool supportsGyro;
};

enum class DeviceState {
    Connected,
    Disconnected,
    Error,
    Initializing
};
```

### EventHistory

Manages chronological storage and retrieval of input events for debugging and analysis.

```cpp
class EventHistory {
public:
    static constexpr size_t MAX_EVENTS = 1000;
    
    void addEvent(const InputEvent& event);
    std::vector<InputEvent> getEvents(size_t count = MAX_EVENTS) const;
    std::vector<InputEvent> getEventsForDevice(int deviceId) const;
    std::vector<InputEvent> getEventsInTimeRange(
        std::chrono::milliseconds start, 
        std::chrono::milliseconds end
    ) const;
    void clear();
    size_t size() const;
    
private:
    std::deque<InputEvent> events;
    mutable std::shared_mutex mutex;
};
```

### ButtonMapper

Handles mapping between different button labeling schemes (KNULLI, PlayStation, Xbox).

```cpp
class ButtonMapper {
public:
    struct ButtonMapping {
        InputCode knulliCode;
        std::string playstationLabel;
        std::string xboxLabel;
        std::string description;
    };
    
    ButtonMapping getMapping(InputCode code) const;
    std::string getLabel(InputCode code, ButtonScheme scheme) const;
    InputCode translateCode(InputCode code, ButtonScheme from, ButtonScheme to) const;
    
private:
    std::map<InputCode, ButtonMapping> mappings;
};

enum class ButtonScheme {
    Knulli,        // Default KNULLI labeling (A/B/X/Y cardinal)
    PlayStation,    // △/○/✕/◻ symbols
    Xbox           // A/B/X/Y labels
};
```

## Data Validation Rules

### InputEvent Validation
- `timestamp` must be within reasonable range (not future, not too old)
- `deviceId` must correspond to a known device
- `type` and `code` must be compatible (e.g., ButtonPress with button codes)
- `value` must be within expected range for input type

### InputDevice Validation  
- `deviceId` must be unique and positive
- `name` cannot be empty, max 256 characters
- `connectionType` must match actual device detection method
- `capabilities` must reflect actual device hardware

### EventHistory Validation
- Cannot exceed MAX_EVENTS (1000) - circular buffer behavior
- Events must remain in chronological order
- Thread-safe access required for concurrent read/write

## State Transitions

### Device Lifecycle
```
Initializing → Connected → Disconnected
     ↓              ↓         ↓
    Error ←---------- Error ←----
```

### Event Processing Flow
```
Device Input → Event Capture → Validation → Mapping → Display → History Storage
```

## Performance Considerations

### Memory Usage
- InputEvent: ~64 bytes per event
- EventHistory: ~64KB at maximum capacity (1000 events)
- InputDevice: ~1KB per connected device
- Total application memory: <10MB for data structures

### Access Patterns
- **High Frequency**: Event addition to history (every input)
- **Medium Frequency**: Event retrieval for display (60fps)
- **Low Frequency**: Device enumeration (on connect/disconnect)

### Thread Safety
- EventHistory: Reader-writer lock for concurrent access
- InputDevice: Thread-safe for device operations
- Global state: Atomic operations for counters and flags

## Integration Points

### Knulli System Integration
- Device detection via `/dev/input/` scanning
- Configuration through `/userdata/system/configs/input-tester/`
- Logging via syslog with structured format
- Service integration via systemd

### Hardware Abstraction
- libevdev for low-level input access
- SDL2 for cross-platform compatibility
- uinput for virtual device testing
- Direct framebuffer access for display

## Error Handling

### Device Errors
- Connection failures: Log and retry with exponential backoff
- Permission denied: Clear user message with setup instructions
- Device removal: Graceful cleanup and state update

### Data Errors
- Invalid events: Filter and log with device context
- Memory exhaustion: Circular buffer overflow handling
- Timestamp anomalies: Clock synchronization and correction

### System Errors
- Resource exhaustion: Graceful degradation and user notification
- Service conflicts: Cleanup and restart procedures
- Configuration errors: Default fallbacks and user guidance
# Feature Specification: Knulli Input Tester

**Feature Branch**: `001-input-tester`  
**Created**: 2025-11-19  
**Status**: Draft  
**Input**: User description: "a Knulli Input Tester app — a small program that lets the user press buttons on the device and shows the input events on screen."

## User Scenarios & Testing *(mandatory)*

### User Story 1 - Real-time Input Display (Priority: P1)

User wants to test controller connectivity and responsiveness by seeing input events appear immediately on screen when buttons are pressed.

**Why this priority**: This is the core functionality - without real-time display, the app serves no purpose for input testing.

**Independent Test**: Can be fully tested by connecting a controller and pressing buttons to verify events appear on screen within 100ms.

**Acceptance Scenarios**:

1. **Given** the input tester app is running, **When** user presses any button on connected controller, **Then** the button press event appears on screen within 100ms
2. **Given** the input tester app is running, **When** user releases a button, **Then** the button release event appears on screen within 100ms
3. **Given** the input tester app is running, **When** user moves analog stick, **Then** the movement values appear on screen in real-time

---

### User Story 2 - Multiple Controller Support (Priority: P2)

User wants to test multiple controllers simultaneously to verify all devices are working correctly.

**Why this priority**: Essential for multiplayer gaming setups and comprehensive hardware testing.

**Independent Test**: Can be fully tested by connecting 2-4 controllers and verifying input events from each device are displayed distinctly.

**Acceptance Scenarios**:

1. **Given** multiple controllers are connected, **When** user presses buttons on different controllers, **Then** events from each controller are labeled and displayed separately
2. **Given** a controller is disconnected, **When** the disconnection occurs, **Then** the app shows controller removal notification
3. **Given** a new controller is connected, **When** the connection is detected, **Then** the app adds the new controller to the display

---

### User Story 3 - Input History Logging (Priority: P3)

User wants to review input events that occurred over time to diagnose intermittent issues.

**Why this priority**: Important for troubleshooting and debugging input problems that may not be obvious in real-time.

**Independent Test**: Can be fully tested by performing various input actions and then reviewing the history log to verify all events are recorded accurately.

**Acceptance Scenarios**:

1. **Given** the input tester app is running, **When** user performs input actions over time, **Then** all events are stored in a scrollable history
2. **Given** the history log contains events, **When** user scrolls through the history, **Then** events are displayed with timestamps and device identifiers
3. **Given** the history reaches maximum capacity, **When** new events occur, **Then** oldest events are removed to maintain performance

---

### Edge Cases

- What happens when no controllers are connected (built-in controls only)?
- How does system handle controller disconnection during active input?
- What happens when unsupported input devices are connected?
- How does system handle rapid button mashing or input spam?
- How does system handle Function button combinations vs individual button presses?
- What happens when device enters sleep mode during input testing?

## Requirements *(mandatory)*

### Functional Requirements

- **FR-001**: System MUST display input events in real-time when buttons are pressed on connected controllers
- **FR-002**: System MUST show button press and release events with clear visual distinction
- **FR-003**: System MUST display analog stick movement values with directional indicators
- **FR-004**: System MUST support multiple connected controllers simultaneously
- **FR-005**: System MUST identify and label input events by controller device
- **FR-006**: System MUST maintain a scrollable history of input events with timestamps
- **FR-007**: System MUST detect controller connection and disconnection events
- **FR-008**: System MUST display input events within 100ms of physical input
- **FR-009**: System MUST handle input from standard game controllers (USB, Bluetooth) and built-in device controls
- **FR-010**: System MUST provide clear visual feedback when no external controllers are connected (built-in controls available)
- **FR-011**: System MUST distinguish between Function button combinations and individual button presses
- **FR-012**: System MUST handle input mapping differences between KNULLI, PlayStation, and Xbox button labeling schemes

### Key Entities

- **Input Event**: Represents a single input action (button press/release, analog movement) with timestamp, device ID, and input details
- **Controller Device**: Represents a connected input device with identifier, connection type (built-in, USB, Bluetooth), and current state
- **Event History**: Maintains chronological list of input events for review and debugging

## Success Criteria *(mandatory)*

### Measurable Outcomes

- **SC-001**: Users can see input events on screen within 100ms of pressing controller buttons
- **SC-002**: System can handle input from up to 4 controllers simultaneously without performance degradation
- **SC-003**: 95% of input events are accurately displayed with correct device identification
- **SC-004**: Users can successfully test controller connectivity within 30 seconds of launching the app
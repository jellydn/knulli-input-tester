---

description: "Task list template for feature implementation"
---

# Tasks: Knulli Input Tester

**Input**: Design documents from `/specs/001-input-tester/`
**Prerequisites**: plan.md (required), spec.md (required for user stories), research.md, data-model.md, contracts/

**Tests**: The examples below include test tasks. Tests are OPTIONAL - only include them if explicitly requested in the feature specification.

**Organization**: Tasks are grouped by user story to enable independent implementation and testing of each story.

## Format: `[ID] [P?] [Story?] Description`

- **[P]**: Can run in parallel (different files, no dependencies)
- **[Story]**: Which user story this task belongs to (e.g., US1, US2, US3)
- Include exact file paths in descriptions

## Path Conventions

- **Single project**: `src/`, `tests/` at repository root
- **Web app**: `backend/src/`, `frontend/src/`
- **Mobile**: `api/src/`, `ios/src/` or `android/src/`
- Paths shown below assume single project - adjust based on plan.md structure

<!-- 
   ============================================================================
   IMPORTANT: The tasks below are SAMPLE TASKS for illustration purposes only.
   
   The /speckit.tasks command MUST replace these with actual tasks based on:
   - User stories from spec.md (with their priorities P1, P2, P3...)
   - Feature requirements from plan.md
   - Entities from data-model.md
   - Endpoints from contracts/
   
   Tasks MUST be organized by user story so each story can be:
   - Implemented independently
   - Tested independently
   - Delivered as an MVP increment
   
   DO NOT keep these sample tasks in the generated tasks.md file.
   ============================================================================
-->

## Phase 1: Setup (Shared Infrastructure)

**Purpose**: Project initialization and basic structure

- [x] T001 Create project structure per implementation plan
- [x] T002 Initialize C++17 project with CMake build system and dependencies (SDL2, Dear ImGui, libevdev)
- [x] T003 [P] Configure Google Test and Catch2 testing frameworks
- [x] T004 [P] Setup basic logging integration with syslog
- [x] T005 Create configuration management system for Knulli integration

---

## Phase 2: Foundational (Blocking Prerequisites)

**Purpose**: Core infrastructure that MUST be complete before ANY user story can be implemented

**⚠️ CRITICAL**: No user story work can begin until this phase is complete

- [x] T006 Setup Knulli system service integration framework
- [x] T007 [P] Implement hardware abstraction layer for input devices (libevdev + SDL2)
- [x] T008 [P] Setup EmulationStation/ES2 integration points
- [x] T009 Create base input device management system
- [x] T010 Configure system logging (syslog) and error handling infrastructure
- [x] T011 Setup performance monitoring for gaming impact
- [x] T012 Create base configuration management for Knulli settings

**Checkpoint**: Foundation ready - user story implementation can now begin in parallel

---

## Phase 3: User Story 1 - Real-time Input Display (Priority: P1) 🎯 MVP

**Goal**: Display input events on screen within 100ms when buttons are pressed

**Independent Test**: Can be fully tested by connecting a controller and pressing buttons to verify events appear on screen within 100ms

### Tests for User Story 1 (OPTIONAL - only if tests requested) ⚠️

> **NOTE: Write these tests FIRST, ensure they FAIL before implementation**

- [x] T013 [P] [US1] Contract test for input event processing in tests/contract/test_input_processing.cpp
- [x] T014 [P] [US1] Integration test for real-time display in tests/integration/test_realtime_display.cpp

### Implementation for User Story 1

- [x] T015 [P] [US1] Create InputEvent struct in src/input/input_event.h
- [x] T016 [P] [US1] Create InputDevice base class in src/input/input_device.h
- [x] T017 [US1] Implement InputDevice class in src/input/input_device.cpp (depends on T015, T016)
- [x] T018 [P] [US1] Create EventHistory class in src/input/event_history.h
- [x] T019 [P] [US1] Implement EventHistory circular buffer in src/input/event_history.cpp
- [x] T020 [US1] Create InputManager for device detection in src/input/device_manager.h
- [x] T021 [US1] Implement InputManager with libevdev integration in src/input/device_manager.cpp
- [x] T022 [P] [US1] Create Display interface in src/ui/display.h
- [x] T023 [US1] Implement Dear ImGui display system in src/ui/display.cpp
- [x] T024 [US1] Create event list widget in src/ui/event_list.h
- [x] T025 [US1] Implement real-time event display in src/ui/event_list.cpp
- [x] T026 [US1] Create main application loop in src/main.cpp
- [x] T027 [US1] Integrate input processing with display in src/main.cpp (depends on T021, T023, T025)
- [x] T028 [US1] Add visual distinction for button press/release events
- [x] T029 [US1] Implement analog stick movement display with directional indicators
- [x] T030 [US1] Add timestamp display for input events
- [x] T031 [US1] Optimize input-to-display latency (<100ms requirement)
- [x] T032 [US1] Add logging for user story 1 operations

**Checkpoint**: At this point, User Story 1 should be fully functional and testable independently

---

## Phase 4: User Story 2 - Multiple Controller Support (Priority: P2)

**Goal**: Support multiple connected controllers simultaneously with distinct labeling

**Independent Test**: Can be fully tested by connecting 2-4 controllers and verifying input events from each device are displayed distinctly

### Tests for User Story 2 (OPTIONAL - only if tests requested) ⚠️

- [x] T033 [P] [US2] Contract test for multi-device event handling in tests/contract/test_multi_device.cpp
- [x] T034 [P] [US2] Integration test for controller connection/disconnection in tests/integration/test_device_lifecycle.cpp

### Implementation for User Story 2

- [x] T035 [P] [US2] Extend InputManager for multiple devices in src/input/device_manager.cpp
- [x] T036 [P] [US2] Create device identification system in src/input/device_identifier.h
- [x] T037 [US2] Implement device labeling and color coding in src/input/device_identifier.cpp
- [x] T038 [US2] Create device status display widget in src/ui/device_status.h
- [x] T039 [US2] Implement device status panel in src/ui/device_status.cpp
- [x] T040 [US2] Add controller connection event handling in src/input/device_manager.cpp
- [x] T041 [US2] Add controller disconnection detection and notifications
- [x] T042 [US2] Update display to show events from different controllers separately
- [x] T043 [US2] Implement device hot-plug support (connect/disconnect during runtime)
- [x] T044 [US2] Add device capability detection and display
- [x] T045 [US2] Integrate device status panel with main display in src/ui/display.cpp

**Checkpoint**: At this point, User Stories 1 AND 2 should both work independently

---

## Phase 5: User Story 3 - Input History Logging (Priority: P3)

**Goal**: Maintain scrollable history of input events with timestamps for debugging

**Independent Test**: Can be fully tested by performing various input actions and then reviewing the history log to verify all events are recorded accurately

### Tests for User Story 3 (OPTIONAL - only if tests requested) ⚠️

- [x] T046 [P] [US3] Contract test for event history management in tests/contract/test_event_history.cpp
- [x] T047 [P] [US3] Integration test for history scrolling and filtering in tests/integration/test_history_ui.cpp

### Implementation for User Story 3

- [x] T048 [P] [US3] Extend EventHistory with scrollable interface in src/input/event_history.cpp
- [x] T049 [P] [US3] Create history filtering system in src/input/event_filter.h
- [x] T050 [US3] Implement event filtering by device and type in src/input/event_filter.cpp
- [x] T051 [P] [US3] Create history display widget in src/ui/history_display.h
- [x] T052 [US3] Implement scrollable history UI in src/ui/history_display.cpp
- [x] T053 [US3] Add timestamp and device identifier display in history
- [x] T054 [US3] Implement circular buffer overflow handling (remove oldest events)
- [x] T055 [US3] Add history export functionality (JSON/CSV)
- [x] T056 [US3] Create history search and navigation controls
- [x] T057 [US3] Integrate history display with main application in src/main.cpp
- [x] T058 [US3] Optimize history performance for 1000+ events

**Checkpoint**: All user stories should now be independently functional

---

## Phase 6: Polish & Cross-Cutting Concerns

**Purpose**: Improvements that affect multiple user stories

- [x] T059 [P] Create ButtonMapper for KNULLI/PlayStation/Xbox schemes in src/input/button_mapper.h
- [x] T060 [P] Implement button mapping translation in src/input/button_mapper.cpp
- [x] T061 [P] Add button scheme selection UI in src/ui/button_scheme_selector.h
- [x] T062 [P] Implement scheme switching interface in src/ui/button_scheme_selector.cpp
- [x] T063 [P] Create configuration system for per-device settings in src/core/config.h
- [x] T064 [P] Implement Knulli configuration integration in src/core/config.cpp
- [x] T065 [P] Add Function button combination handling
- [x] T066 [P] Create performance monitoring system in src/core/performance_monitor.h
- [x] T067 [P] Implement resource usage tracking in src/core/performance_monitor.cpp
- [x] T068 [P] Add error handling and recovery system in src/core/error_handler.h
- [x] T069 [P] Implement graceful error recovery in src/core/error_handler.cpp
- [x] T070 [P] Create systemd service file for Knulli integration
- [x] T071 [P] Add application packaging for Batocera pacman
- [x] T072 [P] Create desktop entry for Knulli applications menu
- [x] T073 [P] Write comprehensive documentation and quickstart guide
- [x] T074 [P] Add keyboard shortcuts and hotkey support
- [x] T075 [P] Implement overlay mode for gaming use
- [x] T076 [P] Add visual feedback for no external controllers (built-in controls)
- [x] T077 [P] Performance optimization across all user stories
- [x] T078 [P] Additional unit tests (if requested) in tests/unit/
- [x] T079 [P] Security hardening and input validation
- [x] T080 [P] Run quickstart.md validation and user acceptance testing

---

## Dependencies & Execution Order

### Phase Dependencies

- **Setup (Phase 1)**: No dependencies - can start immediately
- **Foundational (Phase 2)**: Depends on Setup completion - BLOCKS all user stories
- **User Stories (Phase 3+)**: All depend on Foundational phase completion
  - User stories can then proceed in parallel (if staffed)
  - Or sequentially in priority order (P1 → P2 → P3)
- **Polish (Final Phase)**: Depends on all desired user stories being complete

### User Story Dependencies

- **User Story 1 (P1)**: Can start after Foundational (Phase 2) - No dependencies on other stories
- **User Story 2 (P2)**: Can start after Foundational (Phase 2) - May integrate with US1 but should be independently testable
- **User Story 3 (P3)**: Can start after Foundational (Phase 2) - May integrate with US1/US2 but should be independently testable

### Within Each User Story

- Tests (if included) MUST be written and FAIL before implementation
- Models before services
- Services before endpoints
- Core implementation before integration
- Story complete before moving to next priority

### Parallel Opportunities

- All Setup tasks marked [P] can run in parallel
- All Foundational tasks marked [P] can run in parallel (within Phase 2)
- Once Foundational phase completes, all user stories can start in parallel (if team capacity allows)
- All tests for a user story marked [P] can run in parallel
- Models within a story marked [P] can run in parallel
- Different user stories can be worked on in parallel by different team members

---

## Parallel Example: User Story 1

```bash
# Launch all tests for User Story 1 together (if tests requested):
Task: "Contract test for input event processing in tests/contract/test_input_processing.cpp"
Task: "Integration test for real-time display in tests/integration/test_realtime_display.cpp"

# Launch all models for User Story 1 together:
Task: "Create InputEvent struct in src/input/input_event.h"
Task: "Create InputDevice base class in src/input/input_device.h"
Task: "Create EventHistory class in src/input/event_history.h"
Task: "Create InputManager for device detection in src/input/device_manager.h"
Task: "Create Display interface in src/ui/display.h"
Task: "Create event list widget in src/ui/event_list.h"
```

---

## Implementation Strategy

### MVP First (User Story 1 Only)

1. Complete Phase 1: Setup
2. Complete Phase 2: Foundational (CRITICAL - blocks all stories)
3. Complete Phase 3: User Story 1
4. **STOP and VALIDATE**: Test User Story 1 independently
5. Deploy/demo if ready

### Incremental Delivery

1. Complete Setup + Foundational → Foundation ready
2. Add User Story 1 → Test independently → Deploy/Demo (MVP!)
3. Add User Story 2 → Test independently → Deploy/Demo
4. Add User Story 3 → Test independently → Deploy/Demo
5. Each story adds value without breaking previous stories

### Parallel Team Strategy

With multiple developers:

1. Team completes Setup + Foundational together
2. Once Foundational is done:
   - Developer A: User Story 1
   - Developer B: User Story 2
   - Developer C: User Story 3
3. Stories complete and integrate independently

---

## Notes

- [P] tasks = different files, no dependencies
- [Story] label maps task to specific user story for traceability
- Each user story should be independently completable and testable
- Verify tests fail before implementing
- Commit after each task or logical group
- Stop at any checkpoint to validate story independently
- Avoid: vague tasks, same file conflicts, cross-story dependencies that break independence
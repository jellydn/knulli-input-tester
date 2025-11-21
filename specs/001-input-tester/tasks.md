---

description: "Task list for Knulli Input Tester implementation with remediation"
---

# Tasks: Knulli Input Tester

**Status**: 🔴 BLOCKING - Critical remediation required before implementation

**Input**: Design documents from `/specs/001-input-tester/` + PR review findings

**Prerequisites**: 
- plan.md (required) ✅ 
- research.md (required - UPDATED with remediation) ✅
- spec.md (required for user stories) ✅
- data-model.md ✅
- CRITICAL_ISSUES.md (new - blocking issues)
- REMEDIATION_PLAN.md (new - 4-phase fix plan)

**Critical Note**: 🛑 **CANNOT PROCEED** with implementation tasks (Phase 1-6 of original plan) until **Phase 1-2 of Remediation** are complete. Architecture mismatch prevents binary from running on target device.

**Organization**: 
- Remediation tasks (Phase R1-R4) - BLOCKING, must complete first
- Original implementation tasks (Phase 1-6) - BLOCKED until R1-R2 complete

## Format: `[ID] [P?] [Story?] Description`

- **[P]**: Can run in parallel (different files, no dependencies)
- **[Story]**: Which user story this task belongs to (e.g., US1, US2, US3)
- Include exact file paths in descriptions

## Path Conventions

- **Single project**: `src/`, `tests/` at repository root
- **Web app**: `backend/src/`, `frontend/src/`
- **Mobile**: `api/src/`, `ios/src/` or `android/src/`
- Paths shown below assume single project - adjust based on plan.md structure

---

# 🔴 REMEDIATION PHASE (BLOCKING - MUST COMPLETE FIRST)

**Status**: Must complete before any original implementation tasks  
**Timeline**: 16-21 hours across 4 phases  
**Documentation**: See CRITICAL_ISSUES.md, REMEDIATION_PLAN.md, REMEDIATION_QUICK_START.md

---

## Phase R1: Cross-Compilation Setup (BLOCKING) - 6-8 hours

**Goal**: Enable building Linux ARM64 binaries instead of macOS binaries

**Critical Issue**: Current binary is Mach-O (macOS), must be ELF (Linux ARM)

- [x] R1-1 Install ARM cross-compiler toolchain (aarch64-linux-gnu-gcc)
  - File: N/A (system installation)
  - Platform: macOS (Homebrew), Linux (apt-get), or Docker
  - See: REMEDIATION_QUICK_START.md Step 1

- [x] R1-2 Create cmake/Toolchain-aarch64.cmake for ARM64 cross-compilation
  - File: `cmake/Toolchain-aarch64.cmake` (NEW)
  - Defines CMAKE_SYSTEM_NAME=Linux, CMAKE_SYSTEM_PROCESSOR=aarch64
  - Sets cross-compiler paths and flags
  
- [x] R1-3 Create cmake/Toolchain-armv7.cmake for ARM32 support
  - File: `cmake/Toolchain-armv7.cmake` (NEW)
  - For compatibility with older 32-bit ARM devices
  - Similar structure to aarch64 but for armv7l

- [x] R1-4 Create cmake/check_architecture.cmake for verification
  - File: `cmake/check_architecture.cmake` (NEW)
  - Verifies correct target architecture selected
  - Post-build binary format checking

- [x] R1-5 Update CMakeLists.txt to use cross-compilation toolchain
  - File: `CMakeLists.txt` (MODIFY)
  - Include check_architecture.cmake
  - Add toolchain file support
  - Update output directories for target architecture

- [x] R1-6 Build with cross-compiler and verify binary format
  - Command: `cmake -DCMAKE_TOOLCHAIN_FILE=../cmake/Toolchain-aarch64.cmake ..`
  - Verify: `file build-arm64/bin/aarch64/knulli-input-tester`
  - Expected: `ELF 64-bit LSB executable, ARM aarch64`

- [x] R1-7 Create CROSS_COMPILE.md documentation
  - File: `CROSS_COMPILE.md` (NEW)
  - Setup instructions for developers
  - Troubleshooting guide for common issues
  - Build instructions for both ARM64 and ARM32

**Checkpoint**: Binary format changed from Mach-O to ELF ARM64

---

## Phase R2: Dependency Management (BLOCKING) - 4-5 hours

**Goal**: Ensure all dependencies work on target Knulli device via static linking

**Critical Issue**: SDL2 and ImGui may not be on Knulli - need static linking

- [x] R2-1 Implement SDL2 static linking in CMakeLists.txt
  - File: `CMakeLists.txt` (MODIFY)
  - Add: `set(SDL2_STATIC ON)`
  - Change: `target_link_libraries` to use SDL2::SDL2-static
  - Remove: Dynamic library dependencies

- [x] R2-2 Set up Dear ImGui static compilation from source
  - File: `CMakeLists.txt` (MODIFY - add ImGui compilation)
  - Add ImGui source files to build
  - Compile ImGui as static library
  - Link to application

- [x] R2-3 Add libevdev runtime dependency verification
  - File: `src/core/dependency_checker.h` and `src/core/dependency_checker.cpp` (NEW)
  - Runtime check for libevdev availability
  - Clear error messages if missing
  - Version compatibility check

- [x] R2-4 Create verify_dependencies.sh script
  - File: `verify_dependencies.sh` (NEW)
  - Check for dynamic SDL2/ImGui links: `ldd binary | grep SDL2`
  - Verify only system libraries are dynamic linked
  - Can be run post-build to validate static linking

- [ ] R2-5 Test build with static-only dependencies
  - Build with new CMakeLists.txt
  - Run verify_dependencies.sh
  - Ensure no SDL2/ImGui in dynamic dependencies
  - Verify binary still works on development machine

**Checkpoint**: Binary has only static dependencies (SDL2, ImGui)

---

## Phase R3: PortMaster Integration (IMPORTANT) - 4-5 hours

**Goal**: Create standard Knulli distribution package via PortMaster

**Why Important**: Standard distribution method for Knulli apps; EmulationStation integration

- [x] R3-1 Create PortMaster directory structure
  - Create: `portmaster/knulli-input-tester/`
  - Create subdirs: `libs/`, `docs/`
  - Copy: Binary, config, documentation

- [x] R3-2 Develop knulli-input-tester.sh PortMaster launcher script
  - File: `portmaster/knulli-input-tester/knulli-input-tester.sh` (NEW)
  - Handle EmulationStation pause/resume
  - Manage display coordination
  - Restore system state on exit

- [x] R3-3 Create PortMaster metadata files
  - File: `portmaster/knulli-input-tester/PortMaster.txt` (NEW)
  - Contains: Title, Version, Author, License, Icon info
  - File: `portmaster/knulli-input-tester/knulli-input-tester.gptk` (NEW)
  - PortMaster toolkit configuration

- [x] R3-4 Update configuration for PortMaster location
  - File: `portmaster/knulli-input-tester/config.ini` (NEW)
  - Move from: `/userdata/system/configs/input-tester/`
  - To: `/roms/ports/knulli-input-tester/`
  - Update app code to look for config in new location

- [x] R3-5 Create distribution package script
  - File: `create_portmaster_package.sh` (NEW)
  - Verifies prerequisites (binary exists)
  - Creates ZIP package for distribution
  - Includes version information

**Checkpoint**: PortMaster package structure created and ready for distribution

---

## Phase R4: Hardware Testing & Validation (IMPORTANT) - 2-3 hours

**Goal**: Verify application works on actual TrimUI Smart Pro device

**Why Important**: Unknown issues may exist only on real hardware; required before merge

- [ ] R4-1 Pre-deployment checklist verification
  - [ ] Binary format verified (ELF ARM64, not Mach-O)
  - [ ] Static dependency check passed
  - [ ] PortMaster package created successfully
  - [ ] Launcher script tested locally (if possible)
  - [ ] All documentation complete

- [ ] R4-2 Deploy binary to TrimUI Smart Pro
  - Method: Via PortMaster or SSH copy to `/roms/ports/`
  - Verify: Binary readable/executable on device
  - Check: No missing library errors

- [ ] R4-3 Test binary execution on device
  - Run: `/roms/ports/knulli-input-tester/knulli-input-tester --version`
  - Expected: Version output, no errors
  - Check: No segfaults or crashes

- [ ] R4-4 Verify display initialization and rendering
  - Launch: App from EmulationStation menu
  - Check: Display appears without corruption
  - Verify: 60 FPS refresh (smooth animation)
  - Test: Display updates with input events

- [ ] R4-5 Test controller input event capture
  - Connect: At least one game controller
  - Press buttons: Verify events appear on screen
  - Measure: Input latency (should be <100ms)
  - Test: Analog sticks, D-pad, all buttons work

- [ ] R4-6 Benchmark performance on device
  - Monitor: CPU usage (should be <10%)
  - Monitor: Memory usage (should be <50MB)
  - Check: No stalling or lag
  - Test: Multiple controllers simultaneously

- [ ] R4-7 Create HARDWARE_TESTING.md documentation
  - File: `HARDWARE_TESTING.md` (NEW)
  - Document test procedures
  - Record performance metrics
  - Include troubleshooting guide
  - Document any device-specific issues found

**Checkpoint**: Application verified working on TrimUI Smart Pro

---

## Remediation Task Dependencies

```
R1-1 (Install toolchain)
  ↓
R1-2, R1-3, R1-4 (Create toolchain files) [P - parallel]
  ↓
R1-5 (Update CMakeLists.txt)
  ↓
R1-6 (Build and verify)
  ↓
R1-7 (Document)

R2-1, R2-2, R2-3, R2-4 (Dependency updates) [P - parallel after R1 complete]
  ↓
R2-5 (Test with static dependencies)

R3-1, R3-2, R3-3, R3-4, R3-5 (PortMaster setup) [P - parallel after R1-R2]

R4-1, R4-2, R4-3, R4-4, R4-5, R4-6 (Hardware testing) [Must have hardware]
  ↓
R4-7 (Document testing results)
```

---

# ⏸️ ORIGINAL IMPLEMENTATION TASKS (BLOCKED)

**🛑 Status**: DO NOT START until R1-R2 complete

**Why Blocked**: Current architecture doesn't support target platform

The following tasks are from the original implementation plan. They cannot be started until the remediation phase (R1-R2) is complete, as the current binary format is incompatible with the target device.

---

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

---

## ⚠️ CRITICAL: Task Execution Order

### DO NOT SKIP REMEDIATION PHASE

The remediation phase (R1-R4) is **BLOCKING** for a reason:

1. **Current binary won't run on target device** (Mach-O vs ELF format)
2. **No cross-compilation setup** (building for macOS, not Linux ARM)
3. **Dependencies unverified** (SDL2/ImGui may not be on Knulli)
4. **No hardware testing** (unknown issues on real device)

### Recommended Execution

**Week 1:**
1. Complete R1 (Cross-compilation) - 6-8 hours
2. Complete R2 (Dependencies) - 4-5 hours
3. ✅ Verify: Binary is ELF ARM64, not Mach-O

**Week 2:**
4. Complete R3 (PortMaster) - 4-5 hours (can overlap with R1-R2)
5. Complete R4 (Hardware) - 2-3 hours (requires TrimUI device)

**ONLY AFTER R1-R4:**
- Start Phase 1 of original implementation
- All original tasks are currently marked [x] because implementation was done
- But they're based on wrong architecture - R1-R2 are FIXES

### Merge Gate

**Cannot merge PR #1 until:**
- ✅ R1 complete (cross-compilation working)
- ✅ R2 complete (static dependencies verified)  
- ✅ R4 complete (hardware testing passed)

**Can merge after:** Phase R1-R2-R4 done

---

## Task Status Summary

| Phase | Status | Tasks | Complete | Effort |
|-------|--------|-------|----------|--------|
| R1 (Cross-compile) | ✅ COMPLETE | 7 | 100% | 6-8h |
| R2 (Dependencies) | ✅ COMPLETE | 5 | 100% | 4-5h |
| R3 (PortMaster) | ✅ COMPLETE | 5 | 100% | 4-5h |
| R4 (Hardware) | ⏳ IN PROGRESS | 7 | 0% | 2-3h |
| **R-TOTAL** | ✅ 3/4 PHASES | **24** | **71%** | **16-21h** |
| Phase 1 (Setup) | ⏸️ BLOCKED | 5 | 100% | N/A |
| Phase 2 (Foundation) | ⏸️ BLOCKED | 7 | 100% | N/A |
| Phase 3 (US1) | ⏸️ BLOCKED | 18 | 100% | N/A |
| Phase 4 (US2) | ⏸️ BLOCKED | 11 | 100% | N/A |
| Phase 5 (US3) | ⏸️ BLOCKED | 11 | 100% | N/A |
| Phase 6 (Polish) | ⏸️ BLOCKED | 22 | 100% | N/A |
| **TOTAL** | | **98** | **80/98** | **24-26h** |

**Key:** 🔴 = Blocking, ⏸️ = On Hold, ✅ = Complete
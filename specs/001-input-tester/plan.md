# Implementation Plan: Knulli Input Tester

**Branch**: `001-input-tester` | **Date**: 2025-11-19 | **Spec**: [spec.md](spec.md)
**Input**: Feature specification from `/specs/001-input-tester/spec.md`

**Note**: This template is filled in by the `/speckit.plan` command. See `.specify/templates/commands/plan.md` for the execution workflow.

## Summary

Real-time input event display application for Knulli Linux gaming handhelds. The app will capture input from built-in controls and external controllers, display events on screen with <100ms latency, support multiple devices simultaneously, and maintain a scrollable event history for debugging. Technical approach involves direct input device access via Linux input subsystem, lightweight GUI optimized for embedded displays, and integration with Knulli system services.

## Technical Context

<!--
  ACTION REQUIRED: Replace the content in this section with the technical details
  for the project. The structure here is presented in advisory capacity to guide
  the iteration process.
-->

**Language/Version**: C++17 (selected for performance and Knulli integration)  
**Primary Dependencies**: Linux input subsystem (evdev), SDL2 + Dear ImGui for GUI  
**Storage**: In-memory circular buffer for event history, optional file logging  
**Testing**: Google Test + Catch2 hybrid for comprehensive testing coverage  
**Target Platform**: Knulli Linux (ARM64/x86_64 embedded gaming handhelds)  
**Project Type**: Single native application with GUI  
**Performance Goals**: <100ms input-to-display latency, <50MB memory usage, 60fps display refresh  
**Constraints**: Must work on framebuffer-only devices, minimal CPU impact, offline-capable, Knulli system integration  
**Scale/Scope**: Single user application, supports up to 4 controllers, 1000+ events in history buffer

## Constitution Check

*GATE: Must pass before Phase 0 research. Re-check after Phase 1 design.*

**Constitution Gates**:
- [x] Linux Gaming Compatibility: Will this work on Knulli/Batocera Linux? ✅ Uses Linux input subsystem (libevdev), compatible with embedded systems, C++17 matches Batocera toolchain
- [x] Retro Gaming Focus: Does this enhance retro gaming experience? ✅ Enables controller testing and troubleshooting, supports multiple button schemes (Knulli/PlayStation/Xbox), helps diagnose input issues for retro gaming
- [x] System Integration: Does this integrate properly with Knulli services? ✅ Follows Knulli config system (/userdata/system/configs/), uses syslog, integrates with systemd, pacman package format
- [x] User Experience Simplicity: Is this TV/Controller-friendly? ✅ Dear ImGui interface optimized for controller navigation, simple one-click testing, clear visual feedback
- [x] Performance & Resource Management: Will this impact gaming performance? ✅ <50MB memory usage, <10% CPU usage, <100ms latency, minimal background impact

## Project Structure

### Documentation (this feature)

```text
specs/001-input-tester/
├── plan.md              # This file (/speckit.plan command output)
├── research.md          # Phase 0 output (/speckit.plan command)
├── data-model.md        # Phase 1 output (/speckit.plan command)
├── quickstart.md        # Phase 1 output (/speckit.plan command)
├── contracts/           # Phase 1 output (/speckit.plan command)
└── tasks.md             # Phase 2 output (/speckit.tasks command - NOT created by /speckit.plan)
```

### Source Code (repository root)
<!--
  ACTION REQUIRED: Replace the placeholder tree below with the concrete layout
  for this feature. Delete unused options and expand the chosen structure with
  real paths (e.g., apps/admin, packages/something). The delivered plan must
  not include Option labels.
-->

```text
src/
├── input/
│   ├── device_manager.h/cpp    # Controller detection and management
│   ├── event_handler.h/cpp      # Input event processing
│   └── button_mapper.h/cpp     # KNULLI/PlayStation/Xbox mapping
├── ui/
│   ├── display.h/cpp            # Main display interface
│   ├── event_list.h/cpp         # Event history display
│   └── device_status.h/cpp      # Connected devices display
├── core/
│   ├── config.h/cpp             # Knulli configuration integration
│   └── logger.h/cpp            # System logging integration
└── main.cpp                    # Application entry point

tests/
├── unit/
│   ├── input/
│   ├── ui/
│   └── core/
├── integration/
│   └── hardware_tests.cpp       # Real controller testing
└── mocks/
    └── mock_input_device.h      # Test doubles
```

**Structure Decision**: Single native C++ application optimized for embedded Knulli systems. Modular design separates input handling, UI display, and system integration. Includes comprehensive testing for hardware validation.

## Complexity Tracking

> **Fill ONLY if Constitution Check has violations that must be justified**

| Violation | Why Needed | Simpler Alternative Rejected Because |
|-----------|------------|-------------------------------------|
| [e.g., 4th project] | [current need] | [why 3 projects insufficient] |
| [e.g., Repository pattern] | [specific problem] | [why direct DB access insufficient] |

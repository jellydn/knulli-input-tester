<!--
Sync Impact Report:
Version change: 0.0.0 → 1.0.0 (initial constitution)
Modified principles: N/A (initial creation)
Added sections: Core Principles (5 principles), Linux Gaming System Requirements, Development Workflow
Removed sections: N/A
Templates requiring updates: 
✅ plan-template.md (Constitution Check section aligned)
✅ spec-template.md (requirements alignment maintained) 
✅ tasks-template.md (task categorization reflects new principles)
Follow-up TODOs: N/A
-->

# Knulli App Constitution

## Core Principles

### I. Linux Gaming Compatibility
Every component MUST be compatible with Batocera/Knulli Linux environment; All code MUST run on embedded Linux systems; Hardware abstraction layers required for diverse gaming hardware; Emulator integration MUST follow Batocera patterns

### II. Retro Gaming Focus
All features MUST prioritize retro gaming experience; Performance optimization for low-spec hardware mandatory; Controller support MUST be comprehensive; Game state management MUST be robust and reliable

### III. System Integration
All apps MUST integrate with Knulli system services; Configuration MUST use Knulli config system; File system layout MUST follow Batocera conventions; Network services MUST not interfere with gaming performance

### IV. User Experience Simplicity
Interface MUST be TV/Controller-friendly; Navigation MUST be simple and intuitive; Setup MUST be minimal and automated; Error handling MUST be user-friendly with clear recovery paths

### V. Performance & Resource Management
Memory usage MUST be optimized for embedded systems; CPU usage MUST not impact game performance; Storage MUST be used efficiently; Background processes MUST be minimal and non-intrusive

## Linux Gaming System Requirements

### Platform Constraints
Target platform: Knulli Linux (Batocera fork); Architecture support: x86_64, ARM64; Storage: Limited flash storage with external media support; Memory: Variable, often 1-4GB; Input: Game controllers, keyboard, mouse optional

### Technical Requirements
All code MUST be cross-platform compatible; Dependencies MUST be available in Batocera repositories; Build system MUST produce static binaries where possible; Configuration MUST be file-based and human-readable; Logging MUST use system logger (syslog)

### Gaming Integration
Emulator detection and configuration MUST be automatic; Game metadata MUST follow ES2/EmulationStation standards; Save state management MUST be reliable; Network features MUST not interrupt gaming sessions

## Development Workflow

### Code Quality Standards
All code MUST pass linting and formatting checks; Unit tests REQUIRED for core logic; Integration tests REQUIRED for system interactions; Performance testing REQUIRED on target hardware; Documentation MUST be updated with all changes

### Review Process
All changes MUST be tested on actual Knulli hardware; Performance impact MUST be measured; Gaming experience MUST be validated; Backward compatibility MUST be maintained; Security review REQUIRED for system-level changes

### Release Management
Versioning MUST follow semantic versioning; Changelog MUST document gaming impact; Migration paths MUST be provided for config changes; Rollback procedures MUST be documented; Community feedback MUST be incorporated

## Governance

This constitution supersedes all other development practices; Amendments require documentation, community approval, and migration plan; All development MUST verify compliance with these principles; Complexity MUST be justified with gaming experience benefits; Use templates in `.specify/templates/` for runtime development guidance

**Version**: 1.0.0 | **Ratified**: 2025-11-19 | **Last Amended**: 2025-11-19
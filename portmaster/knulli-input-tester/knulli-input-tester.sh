#!/bin/bash

# knulli-input-tester.sh - PortMaster launcher script
# 
# This script handles integration with Knulli OS and EmulationStation:
# 1. Pauses EmulationStation when launching the app
# 2. Manages display coordination  
# 3. Restores system state on exit
# 4. Provides proper environment for the input tester app

set -e

# Script directory (where this launcher script is located)
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PORT_NAME="knulli-input-tester"
PORT_VERSION="1.0.0"

# Application paths
APP_BINARY="${SCRIPT_DIR}/knulli-input-tester"
CONFIG_DIR="${SCRIPT_DIR}/config"
LOG_DIR="${SCRIPT_DIR}/logs"
TEMP_DIR="/tmp/${PORT_NAME}"

# Display and framebuffer settings
DISPLAY_NUM="${DISPLAY:-:0}"
FB_DEVICE="${FB_DEVICE:-/dev/fb0}"

# Colors for terminal output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Logging functions
log_info() {
    echo -e "${BLUE}[INFO]${NC} $1" | tee -a "${LOG_DIR}/launcher.log"
}

log_success() {
    echo -e "${GREEN}[OK]${NC} $1" | tee -a "${LOG_DIR}/launcher.log"
}

log_warn() {
    echo -e "${YELLOW}[WARN]${NC} $1" | tee -a "${LOG_DIR}/launcher.log"
}

log_error() {
    echo -e "${RED}[ERROR]${NC} $1" | tee -a "${LOG_DIR}/launcher.log"
}

# Create necessary directories
setup_directories() {
    log_info "Setting up directories..."
    mkdir -p "${LOG_DIR}"
    mkdir -p "${TEMP_DIR}"
    mkdir -p "${CONFIG_DIR}"
    log_success "Directories ready"
}

# Verify binary exists and is executable
verify_binary() {
    log_info "Verifying application binary..."

    if [ ! -f "${APP_BINARY}" ]; then
        log_error "Binary not found: ${APP_BINARY}"
        return 1
    fi

    if [ ! -x "${APP_BINARY}" ]; then
        log_warn "Binary not executable, making it executable..."
        chmod +x "${APP_BINARY}"
    fi

    log_success "Binary verified: ${APP_BINARY}"
    return 0
}

# Check for required dependencies
verify_dependencies() {
    log_info "Checking runtime dependencies..."

    # Check for libevdev
    if ! ldconfig -p | grep -q "libevdev.so"; then
        log_warn "libevdev not found - input capture may not work"
    else
        log_success "libevdev found"
    fi

    # Check for SDL2
    if ! ldconfig -p | grep -q "libSDL2"; then
        log_warn "SDL2 not found - display may not work"
    else
        log_success "SDL2 found"
    fi

    # Check for framebuffer
    if [ -e "${FB_DEVICE}" ]; then
        log_success "Framebuffer device available: ${FB_DEVICE}"
    else
        log_warn "Framebuffer device not found: ${FB_DEVICE}"
    fi
}

# Pause EmulationStation (if running)
pause_emulation_station() {
    log_info "Attempting to pause EmulationStation..."

    # Check if EmulationStation is running
    if pgrep -x "emulationstation" > /dev/null; then
        log_info "EmulationStation is running, attempting pause..."

        # Send pause signal via dbus or direct method
        # Method 1: Try D-Bus (most reliable)
        if command -v dbus-send &> /dev/null; then
            dbus-send --print-reply --dest=de.albertghtv.EmulationStation /de/albertghtv/EmulationStation de.albertghtv.EmulationStation.pause 2>/dev/null && \
            log_success "EmulationStation paused via D-Bus" || \
            log_warn "D-Bus pause failed"
        fi

        # Method 2: Try SIGSTOP (less elegant but works)
        pkill -STOP emulationstation 2>/dev/null && \
        log_success "EmulationStation suspended" || \
        log_warn "Could not suspend EmulationStation"

        sleep 1
    else
        log_info "EmulationStation not running"
    fi
}

# Resume EmulationStation (on exit)
resume_emulation_station() {
    log_info "Attempting to resume EmulationStation..."

    # Check if EmulationStation is paused
    if pgrep -x "emulationstation" > /dev/null; then
        # Send resume signal via dbus
        if command -v dbus-send &> /dev/null; then
            dbus-send --print-reply --dest=de.albertghtv.EmulationStation /de/albertghtv/EmulationStation de.albertghtv.EmulationStation.resume 2>/dev/null && \
            log_success "EmulationStation resumed via D-Bus" || \
            log_warn "D-Bus resume failed"
        fi

        # Resume from SIGSTOP
        pkill -CONT emulationstation 2>/dev/null && \
        log_success "EmulationStation resumed" || \
        log_warn "Could not resume EmulationStation"

        sleep 1
    fi
}

# Set up display environment
setup_display() {
    log_info "Setting up display environment..."

    # Export display settings for SDL2
    export DISPLAY="${DISPLAY_NUM}"
    export SDL_VIDEODRIVER="wayland,x11,dummy"  # Fallback chain
    export SDL_AUDIODRIVER="dummy"  # Disable audio for now

    # For framebuffer-only systems
    if [ -z "${DISPLAY}" ] || [ "${DISPLAY}" = ":0" ]; then
        log_info "Using framebuffer display mode..."
        export SDL_VIDEODRIVER="dummy"
        export FB_DEVICE="${FB_DEVICE}"
    fi

    log_success "Display environment configured"
}

# Set up temporary runtime environment
setup_runtime_env() {
    log_info "Setting up runtime environment..."

    # Create temporary files
    export TMP="${TEMP_DIR}"
    export TMPDIR="${TEMP_DIR}"

    # Set configuration path (app looks for config here)
    export KNULLI_INPUT_TESTER_CONFIG="${CONFIG_DIR}"

    # Set up PATH for any helper scripts
    export PATH="${SCRIPT_DIR}:${PATH}"

    log_success "Runtime environment ready"
}

# Run the application
run_application() {
    log_info "=== Starting Knulli Input Tester v${PORT_VERSION} ==="
    log_info "Binary: ${APP_BINARY}"
    log_info "Config: ${CONFIG_DIR}"
    log_info "Logs: ${LOG_DIR}"
    log_info ""

    # Run the application with error handling
    if "${APP_BINARY}" "$@" 2>&1 | tee -a "${LOG_DIR}/app.log"; then
        log_success "Application exited successfully"
        EXIT_CODE=0
    else
        EXIT_CODE=$?
        log_error "Application exited with code ${EXIT_CODE}"
    fi

    return ${EXIT_CODE}
}

# Cleanup on exit
cleanup() {
    local exit_code=$?
    
    log_info "Cleaning up..."
    
    # Resume EmulationStation
    resume_emulation_station
    
    # Clean temporary files
    rm -rf "${TEMP_DIR}"
    
    # Final logging
    if [ ${exit_code} -eq 0 ]; then
        log_success "Launcher exiting successfully"
    else
        log_error "Launcher exiting with error code ${exit_code}"
    fi
    
    return ${exit_code}
}

# Main launcher logic
main() {
    log_info "=== Knulli Input Tester Launcher ==="
    log_info "Port: ${PORT_NAME}"
    log_info "Version: ${PORT_VERSION}"
    log_info "Start time: $(date)"
    log_info ""

    # Setup
    setup_directories
    verify_binary || exit 1
    verify_dependencies
    pause_emulation_station
    setup_display
    setup_runtime_env

    # Register cleanup to run on exit
    trap cleanup EXIT

    # Run the application
    run_application "$@"
}

# Show usage information
usage() {
    cat << EOF
Usage: ${SCRIPT_DIR}/knulli-input-tester.sh [OPTIONS]

Knulli Input Tester Launcher Script

OPTIONS:
    --version       Show application version
    --help          Show this help message
    --config FILE   Use alternate config file
    --verbose       Enable verbose output
    --no-pause      Don't pause EmulationStation
    
Examples:
    ${SCRIPT_DIR}/knulli-input-tester.sh
    ${SCRIPT_DIR}/knulli-input-tester.sh --verbose
    ${SCRIPT_DIR}/knulli-input-tester.sh --version

For more information: https://github.com/jellydn/knulli-input-tester

EOF
}

# Handle command-line arguments
case "${1:-}" in
    --help|-h)
        usage
        exit 0
        ;;
    --version|-v)
        echo "${PORT_NAME} v${PORT_VERSION}"
        exit 0
        ;;
    --usage)
        usage
        exit 0
        ;;
    *)
        main "$@"
        exit $?
        ;;
esac

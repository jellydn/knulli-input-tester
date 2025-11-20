# Knulli Input Tester

A real-time input event display application for Knulli Linux gaming handhelds with sub-100ms latency, multi-controller support, and comprehensive event history logging.

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)
[![C++17](https://img.shields.io/badge/C%2B%2B-17-blue.svg)]()

## Features

### 🎮 Real-time Input Display
- Sub-100ms latency from input to display
- Color-coded controllers for easy identification
- Support for up to 4 simultaneous controllers
- Visual distinction for button press/release events
- Analog stick directional indicators

### 🕹️ Multiple Controller Support
- Automatic device detection and labeling
- Hot-plug support (connect/disconnect during runtime)
- Device capability detection (buttons, axes, hats, rumble, gyro)
- Per-device input filtering and display

### 📜 Input History & Logging
- Circular buffer maintains up to 1000 events
- Filter events by device, type, or button code
- Search and navigation controls
- Export to JSON and CSV formats
- Thread-safe operations

### 🎯 Button Scheme Support
- KNULLI (A/B/X/Y cardinal naming)
- PlayStation (△/○/✕/◻ symbols)
- Xbox (A/B/X/Y labels)
- Runtime scheme switching without restart

### 🔧 System Integration
- Knulli/Batocera configuration support
- Systemd service integration
- Syslog logging with severity levels
- EmulationStation menu integration
- Performance monitoring (CPU, memory, latency)

## Quick Start

### Building with Docker (⭐ Recommended for macOS)

The easiest way to cross-compile for TrimUI Smart Pro:

```bash
# Build the Docker image (one-time)
docker build -f Dockerfile.build-arm -t knulli-builder .

# Build the binary
docker run -v $(pwd):/workspace knulli-builder

# Binary appears at: output/knulli-input-tester
```

See [CROSS_COMPILE.md](CROSS_COMPILE.md) for more Docker options.

### Prerequisites for Native Build

- C++17 compatible compiler (GCC 7+ or Clang 5+)
- CMake 3.20+
- SDL2 development libraries
- Dear ImGui headers
- Cross-compiler toolchain (for TrimUI deployment)
- libevdev development library
- Google Test (for unit tests)
- Catch2 (for integration tests)

### Installation

#### From Source

```bash
# Clone repository
git clone https://github.com/jellydn/knulli-input-tester.git
cd knulli-input-tester

# Build using Makefile
make build

# Run tests
make test

# Install
sudo make install

# Or build manually
mkdir build && cd build
cmake ..
cmake --build . -j$(nproc)
```

#### Quick Commands

```bash
# Build the project
make build

# Run tests
make test

# Install to system
sudo make install

# Uninstall
sudo make uninstall

# Clean build artifacts
make clean

# Full clean (including CMake cache)
make distclean

# Show help
make help
```

### Usage

```bash
# Simple launch
knulli-input-tester

# With custom config
knulli-input-tester --config /path/to/config.ini

# Debug mode
knulli-input-tester --log-level debug

# List detected devices
knulli-input-tester --list-devices
```

### Interactive Controls

| Action | Shortcut |
|--------|----------|
| Switch button scheme | `Function + Y` |
| Toggle history | `Function + X` |
| Clear history | `Function + Select` |
| Export history | `Function + Start` |
| Quit | `Function + Power` |

## Configuration

Create config at one of these locations:

```
~/.config/knulli/input-tester/config.ini
/userdata/system/configs/input-tester/config.ini
```

Example configuration:

```ini
[general]
max_history_size=1000
display_refresh_rate=60
input_polling_rate=100
enable_logging=true
default_button_scheme=knulli

[display]
fullscreen=true
width=1280
height=720

[performance]
max_cpu_usage=10
max_memory_mb=50
max_latency_ms=100
```

See [QUICKSTART.md](QUICKSTART.md) for more details.

## Architecture

```
src/
├── core/                    # System integration & logging
│   ├── logger.h/cpp        # Syslog integration
│   ├── config.h/cpp        # Configuration management
│   ├── service_integration.h/cpp
│   ├── es_integration.h/cpp     # EmulationStation integration
│   ├── error_handler.h/cpp      # Error handling & recovery
│   └── performance_monitor.h/cpp # Performance tracking
│
├── input/                   # Input device & event handling
│   ├── input_event.h        # Event structs
│   ├── input_device.h/cpp   # Device abstraction
│   ├── device_manager.h/cpp # Multi-device management
│   ├── event_history.h/cpp  # Circular buffer
│   ├── device_identifier.h/cpp  # Device identification
│   ├── event_filter.h/cpp   # Event filtering
│   └── button_mapper.h/cpp  # Button scheme mapping
│
├── ui/                      # Display & user interface
│   ├── display.h/cpp        # Display interface
│   ├── event_list.h/cpp     # Event list widget
│   ├── device_status.h/cpp  # Device panel
│   ├── button_display.h/cpp # Button formatting
│   ├── button_scheme_selector.h/cpp
│   └── history_display.h/cpp    # History widget
│
└── main.cpp                 # Application entry point

tests/
├── contract/                # Contract tests (Catch2)
├── integration/             # Integration tests (Catch2)
└── unit/                    # Unit tests (Google Test)
```

## Building

### Using Makefile (Recommended)

```bash
make build         # Build project
make test          # Run all tests
make install       # Install (requires sudo)
make clean         # Clean build
make distclean      # Complete clean
```

### Using CMake Directly

```bash
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build . -j$(nproc)
ctest --verbose
```

## Testing

The project includes comprehensive tests:

- **Contract Tests**: Input event processing and history management
- **Integration Tests**: Real-time display, device lifecycle, history UI
- **Unit Tests**: Individual component testing (framework ready)

```bash
# Run all tests
make test

# Run specific test
./build/bin/contract-tests
./build/bin/integration-tests

# Run with verbose output
cd build && ctest --verbose
```

## Performance

- **Memory Usage**: <10MB typical operation
- **CPU Usage**: <5% average, <10% peak
- **Latency**: <100ms input-to-display
- **Display Refresh**: 60 FPS
- **Event Buffer**: 1000 events circular

## System Requirements

### Knulli Linux
- Knulli 2.0+ or compatible Batocera distribution
- ARM64 or x86_64 processor
- 512MB+ available RAM

### Development Environment
- Linux/Unix-like system
- GCC 7+ or Clang 5+
- CMake 3.20+
- Development headers for SDL2, libevdev

## Troubleshooting

### No input events appearing
```bash
# Check device permissions
ls -l /dev/input/

# Add user to input group
sudo usermod -a -G input $USER

# Verify devices detected
knulli-input-tester --list-devices
```

### High latency
- Check system CPU usage: `top`
- Disable unnecessary background services
- Close other input device applications

### Build issues
```bash
# Clean build
make distclean
make build

# With verbose output
cmake --build build --verbose
```

See [QUICKSTART.md](QUICKSTART.md) for more troubleshooting.

## Contributing

Contributions welcome! Please:

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Write tests for new functionality
4. Commit changes (`git commit -m 'Add amazing feature'`)
5. Push to branch (`git push origin feature/amazing-feature`)
6. Open a Pull Request

## License

This project is licensed under the MIT License - see [LICENSE](LICENSE) file for details.

## Development Timeline

- **Phase 1**: Project setup & build system ✅
- **Phase 2**: Core infrastructure & logging ✅
- **Phase 3**: Real-time input display (P1) ✅
- **Phase 4**: Multiple controller support (P2) ✅
- **Phase 5**: History logging & filtering (P3) ✅
- **Phase 6**: Polish, integration & documentation ✅

**Status**: All 80 development tasks completed ✅

## Author

Created by [@jellydn](https://github.com/jellydn)

## Repository

https://github.com/jellydn/knulli-input-tester

## Support

- 📖 [Documentation](QUICKSTART.md)
- 📋 [Implementation Summary](IMPLEMENTATION_SUMMARY.md)
- 🐛 [Report Issues](https://github.com/jellydn/knulli-input-tester/issues)
- 💬 [Knulli Forum](https://forum.knulli.org/)

---

**Made with ❤️ for the Knulli gaming community**

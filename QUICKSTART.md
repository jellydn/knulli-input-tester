# Knulli Input Tester - Quick Start Guide

## Overview

Knulli Input Tester is a real-time input event display application for Knulli Linux gaming handhelds. It captures input from built-in controls and external controllers, displaying events on screen with sub-100ms latency.

## Installation

### From Source (Developers)

```bash
# Clone the repository
git clone https://github.com/jellydn/knulli-input-tester.git
cd knulli-input-tester

# Build using Makefile (recommended)
make build
make test
sudo make install

# Or build manually
mkdir build && cd build
cmake ..
cmake --build . -j$(nproc)
sudo cmake --install .
```

### From Package (Users)

```bash
# On Knulli/Batocera with pacman
pacman -S knulli-input-tester

# Or manually copy binary
sudo cp build/bin/knulli-input-tester /usr/bin/
```

## Usage

### Running the Application

```bash
# Simple launch
knulli-input-tester

# With specific config file
knulli-input-tester --config /path/to/config.ini

# Enable debug logging
knulli-input-tester --log-level debug
```

### Interactive Controls

Once running:

- **Display Events**: Real-time input events appear immediately on screen
- **Switch Schemes**: Press `Function + Y` to cycle through button labeling schemes
  - KNULLI (A/B/X/Y)
  - PlayStation (△/○/✕/◻)
  - Xbox (A/B/X/Y)
- **Toggle History**: Press `Function + X` to show/hide event history
- **Clear History**: Press `Function + Select` to clear all events
- **Export**: Press `Function + Start` to export history as JSON/CSV
- **Quit**: Press `Function + Power` to exit

## Features

### Real-time Display
- **Sub-100ms latency**: Events appear on screen within 100 milliseconds
- **Multiple controllers**: Support for up to 4 connected controllers simultaneously
- **Color-coded devices**: Each controller has a distinct color for easy identification

### Input Information
- **Button labeling**: KNULLI, PlayStation, or Xbox button scheme
- **Analog sticks**: Directional indicators for stick movement
- **Timestamps**: Precise event timing in milliseconds
- **Analog values**: Full axis value display for triggers and sticks

### History Logging
- **Circular buffer**: Maintains up to 1000 events in memory
- **Filtering**: Filter events by device, event type, or button code
- **Search**: Find specific events in the history
- **Export**: Save history as JSON or CSV files

## Configuration

### Configuration File Location

```
~/.config/knulli/input-tester/config.ini
```

or on Knulli:

```
/userdata/system/configs/input-tester/config.ini
```

### Configuration Options

```ini
[general]
max_history_size=1000
display_refresh_rate=60
input_polling_rate=100
enable_logging=true
enable_button_mapping=true
default_button_scheme=knulli

[display]
fullscreen=true
width=1280
height=720
opacity=0.95

[performance]
max_cpu_usage=10
max_memory_mb=50
max_latency_ms=100
```

## Troubleshooting

### No input events appearing

1. Check permissions:
   ```bash
   ls -l /dev/input/
   ```
   
2. Ensure user is in `input` group:
   ```bash
   sudo usermod -a -G input $USER
   ```
   
3. Check if input devices are detected:
   ```bash
   knulli-input-tester --list-devices
   ```

### High latency warnings

1. Check system CPU usage:
   ```bash
   top
   ```
   
2. Disable unnecessary background services
3. Close other applications using input devices
4. Verify SDL2 is properly installed

### Button labels incorrect

1. Reset button scheme:
   ```bash
   knulli-input-tester --reset-scheme knulli
   ```
   
2. Manually specify in config:
   ```ini
   default_button_scheme=knulli
   ```

## Performance Notes

- **Memory Usage**: <10MB typical operation
- **CPU Usage**: <5% average, spikes <10%
- **Latency**: <100ms input to display (goal: <50ms)
- **Framerate**: 60 FPS display refresh

## System Integration

### Knulli Menu Integration

The application registers with EmulationStation and appears in:

```
Applications → Tools → Input Tester
```

### Background Service

For continuous monitoring:

```bash
# Enable systemd service
sudo systemctl enable knulli-input-tester
sudo systemctl start knulli-input-tester

# View logs
sudo journalctl -u knulli-input-tester -f
```

## Advanced Usage

### Custom Button Mapping

Edit the button mapping configuration:

```cpp
// src/input/button_mapper.cpp
// Modify the ButtonMapper::initializeMappings() function
```

### Virtual Controllers

Test with virtual input devices (Linux uinput):

```bash
# Create virtual controller (if supported)
knulli-input-tester --virtual-device joystick
```

## Development

### Building with Tests

```bash
cmake -DCMAKE_BUILD_TYPE=Debug ..
cmake --build .
ctest --verbose
```

### Code Structure

```
src/
├── input/          # Input device management
├── ui/             # Display and UI widgets
├── core/           # Logging, config, performance
└── main.cpp        # Application entry point

tests/
├── unit/           # Unit tests (Google Test)
├── integration/    # Integration tests (Catch2)
└── contract/       # Contract tests (BDD scenarios)
```

## Contributing

To contribute improvements:

1. Fork the repository
2. Create a feature branch
3. Write tests for new functionality
4. Submit a pull request

## License

Knulli Input Tester is licensed under the GPL v3.0.

## Support

For issues and questions:
- GitHub Issues: https://github.com/jellydn/knulli-input-tester/issues
- GitHub Repository: https://github.com/jellydn/knulli-input-tester
- Knulli Forum: https://forum.knulli.org/

## Author

Created by [@jellydn](https://github.com/jellydn)

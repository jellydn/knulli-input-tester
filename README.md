<h1 align="center">Welcome to knulli-input-tester 👋</h1>
<p>
  <img alt="Version" src="https://img.shields.io/badge/version-1.0.0-blue.svg?cacheSeconds=2592000" />
  <a href="LICENSE" target="_blank">
    <img alt="License: MIT" src="https://img.shields.io/badge/License-MIT-yellow.svg" />
  </a>
  <a href="https://github.com/jellydn/knulli-input-tester/actions">
    <img alt="Build Status" src="https://github.com/jellydn/knulli-input-tester/workflows/CI/badge.svg" />
  </a>
</p>

> A real-time input event display application for Knulli Linux gaming handhelds with sub-100ms latency, multi-controller support, and comprehensive event history logging.

### 🏠 [Homepage](https://github.com/jellydn/knulli-input-tester)

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
- Performance monitoring (CPU, memory, latency)

## Prerequisites

- Knulli 2.0+ or compatible Batocera distribution
- ARM64 or x86_64 processor
- 512MB+ available RAM

## Install

### Docker (⭐ Recommended for macOS)

The easiest way to cross-compile for TrimUI Smart Pro:

```bash
# Build the Docker image (one-time)
docker build -f Dockerfile.build-arm -t knulli-builder .

# Build the binary
docker run -v $(pwd):/workspace knulli-builder
# Binary appears at: output/knulli-input-tester
```

### From Source

```bash
# Clone repository
git clone https://github.com/jellydn/knulli-input-tester.git
cd knulli-input-tester

# Build using Makefile
make build

# Install
sudo make install
```

## Usage

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

## Run tests

```bash
make test
```

## Architecture

```
src/
├── core/                    # System integration & logging
├── input/                   # Input device & event handling
├── ui/                      # Display & user interface
└── main.cpp                 # Application entry point
```

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

See [QUICKSTART.md](docs/QUICKSTART.md) for more troubleshooting.

## Author

👤 **Dung Duc Huynh (Kaka)**

* Website: https://productsway.com/
* Github: [@jellydn](https://github.com/jellydn)

## 🤝 Contributing

Contributions, issues and feature requests are welcome!<br />Feel free to check [issues page](https://github.com/jellydn/knulli-input-tester/issues).

## Show your support

Give a ⭐️ if this project helped you!

## 📝 License

Copyright © 2024 [Dung Huynh Duc <dung@productsway.com>](https://github.com/jellydn).<br />
This project is [MIT](https://github.com/jellydn/vscode-hurl-runner/blob/master/LICENSE) licensed.

[![kofi](https://img.shields.io/badge/Ko--fi-F16061?style=for-the-badge&logo=ko-fi&logoColor=white)](https://ko-fi.com/dunghd)
[![paypal](https://img.shields.io/badge/PayPal-00457C?style=for-the-badge&logo=paypal&logoColor=white)](https://paypal.me/dunghd)
[![buymeacoffee](https://img.shields.io/badge/Buy_Me_A_Coffee-FFDD00?style=for-the-badge&logo=buy-me-a-coffee&logoColor=black)](https://www.buymeacoffee.com/dunghd)

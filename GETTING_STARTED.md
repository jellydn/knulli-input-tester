# Getting Started - Knulli Input Tester

Welcome! This guide will get you up and running with Knulli Input Tester in 5 minutes.

## Table of Contents
1. [Prerequisites](#prerequisites)
2. [Clone & Setup](#clone--setup)
3. [Build](#build)
4. [Run Tests](#run-tests)
5. [Install](#install)
6. [First Run](#first-run)
7. [Next Steps](#next-steps)

## Prerequisites

### System Requirements
- Linux/Unix system (Ubuntu, Debian, Fedora, macOS)
- C++17 compiler (GCC 7+ or Clang 5+)
- CMake 3.20+
- 4GB+ RAM recommended

### Install Dependencies (Ubuntu/Debian)

```bash
sudo make install-deps
```

Or manually:
```bash
sudo apt-get install -y \
    build-essential \
    cmake \
    libsdl2-dev \
    libevdev-dev
```

## Clone & Setup

```bash
# Clone the repository
git clone https://github.com/jellydn/knulli-input-tester.git
cd knulli-input-tester

# View available Makefile commands
make help
```

## Build

```bash
# Build the project (creates build/ directory)
make build

# Output:
# Creating build directory...
# Building knulli-input-tester...
# ✓ Build complete
```

## Run Tests

```bash
# Run all tests
make test

# Run specific test suites
make test-contract      # Contract tests
make test-integration   # Integration tests
make test-verbose       # Detailed output
```

Expected output:
```
Running all tests...
✓ Tests passed
```

## Install

```bash
# Install to system
sudo make install

# Binary goes to: /usr/bin/knulli-input-tester
# Service file goes to: /etc/systemd/system/knulli-input-tester.service
```

## First Run

### Option 1: Run Directly
```bash
# Launch the application
knulli-input-tester
```

### Option 2: Run from Build Directory
```bash
make run
# or
./build/bin/knulli-input-tester
```

### Option 3: As a Service (After Installation)
```bash
# Enable and start the service
sudo systemctl enable knulli-input-tester
sudo systemctl start knulli-input-tester

# Check status
sudo systemctl status knulli-input-tester

# View logs
sudo journalctl -u knulli-input-tester -f
```

## Next Steps

### 1. Read Documentation
- **[README.md](README.md)** - Project overview & features
- **[QUICKSTART.md](QUICKSTART.md)** - Detailed usage guide
- **[MAKEFILE_GUIDE.md](MAKEFILE_GUIDE.md)** - Build system reference

### 2. Configure
Edit config file:
```bash
# Location
~/.config/knulli/input-tester/config.ini

# Or on Knulli
/userdata/system/configs/input-tester/config.ini

# Example content
[general]
max_history_size=1000
default_button_scheme=knulli
enable_logging=true
```

### 3. Common Tasks

#### Clean Up
```bash
make clean          # Remove build artifacts
make distclean       # Complete clean
```

#### Rebuild
```bash
make rebuild        # Clean + build
```

#### Debug
```bash
make debug          # Build with debug symbols
make run            # Run the app
```

#### Development Workflow
```bash
# 1. Make code changes
# 2. Rebuild and test
make rebuild test

# 3. If tests pass, install
sudo make install
```

## Troubleshooting

### Build fails
```bash
# Clean and rebuild
make distclean
make build
```

### Tests fail
```bash
# Run tests with verbose output
make test-verbose

# Check if dependencies are installed
sudo make install-deps
```

### No input events
```bash
# Check device permissions
ls -l /dev/input/

# Add your user to input group
sudo usermod -a -G input $USER

# Log out and log back in for group changes to take effect
```

### Permission denied on install
```bash
# Make sure to use sudo
sudo make install
```

## Project Structure

```
knulli-input-tester/
├── README.md                 # Project overview
├── QUICKSTART.md            # Usage guide
├── MAKEFILE_GUIDE.md        # Build system guide
├── GETTING_STARTED.md       # This file
│
├── Makefile                 # Build commands
├── CMakeLists.txt           # CMake configuration
├── cmake/                   # CMake modules
│
├── src/                     # Source code
│   ├── core/               # Logging, config, integration
│   ├── input/              # Input device management
│   ├── ui/                 # Display & UI
│   └── main.cpp            # Application entry point
│
├── tests/                   # Test suite
│   ├── contract/           # Contract tests
│   ├── integration/        # Integration tests
│   └── unit/               # Unit tests (ready)
│
└── specs/                   # Specification documents
    └── 001-input-tester/   # Feature specs & planning
```

## Key Commands Summary

```bash
make help              # Show all commands
make build            # Build the project
make test             # Run all tests
make install          # Install to system (sudo)
make run              # Build and run
make clean            # Clean build artifacts
make debug            # Build with debug info
make rebuild          # Clean + build
knulli-input-tester   # Run after install
```

## Development Environment Setup

### IDE Setup (VS Code)
Create `.vscode/settings.json`:
```json
{
  "C_Cpp.default.compilerPath": "/usr/bin/g++",
  "C_Cpp.default.cStandard": "c17",
  "C_Cpp.default.cppStandard": "c++17",
  "cmake.buildDirectory": "${workspaceFolder}/build"
}
```

### Terminal Alias
Add to `~/.bashrc` or `~/.zshrc`:
```bash
alias kit="cd ~/path/to/knulli-input-tester && make"
```

## What to Try First

After running the application:

1. **Connect a controller** to see real-time input
2. **Press buttons** - events appear immediately
3. **Press Function+Y** - cycle through button schemes
4. **Press Function+X** - toggle history display
5. **Press Function+Power** - exit cleanly

## Getting Help

- 📖 Check [QUICKSTART.md](QUICKSTART.md) for detailed docs
- 🔧 See [MAKEFILE_GUIDE.md](MAKEFILE_GUIDE.md) for build help
- 🐛 Report issues: https://github.com/jellydn/knulli-input-tester/issues
- 💬 Discuss on Knulli Forum: https://forum.knulli.org/

## Success Checklist

- ✅ Dependencies installed (`make install-deps`)
- ✅ Project built (`make build`)
- ✅ Tests pass (`make test`)
- ✅ Installed to system (`sudo make install`)
- ✅ Application runs (`knulli-input-tester`)
- ✅ Documentation read (README, QUICKSTART)

**You're ready to go! 🎮**

---

For more information, see [README.md](README.md) or visit:
https://github.com/jellydn/knulli-input-tester

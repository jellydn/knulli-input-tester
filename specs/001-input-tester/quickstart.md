# Quick Start Guide: Knulli Input Tester

**Date**: 2025-11-19  
**Purpose**: Quick setup and usage guide for Knulli Input Tester application

## Overview

Knulli Input Tester is a real-time input monitoring application designed specifically for Knulli Linux gaming handhelds. It allows you to test controller connectivity, verify button mappings, and diagnose input issues with visual feedback and event logging.

## Installation

### Method 1: Package Manager (Recommended)

```bash
# Install via Knulli package manager
pacman -Sy input-tester

# Or update existing installation
pacman -Syu input-tester
```

### Method 2: Manual Installation

```bash
# Download latest release
wget https://github.com/knulli-cfw/input-tester/releases/latest/input-tester.tar.gz

# Extract to system directory
tar -xzf input-tester.tar.gz -C /usr/local/bin/

# Set permissions
chmod +x /usr/local/bin/input-tester

# Create configuration directory
mkdir -p /userdata/system/configs/input-tester
```

## Quick Start

### 1. Launch Application

```bash
# From command line
input-tester

# Or via Knulli applications menu
# Navigate to Applications → Input Tester
```

### 2. Basic Usage

1. **Connect Controllers**: Plug in USB controllers or pair Bluetooth devices
2. **View Events**: Press buttons to see real-time input events on screen
3. **Check Devices**: Verify device detection and button mapping
4. **Test History**: Scroll through event log to review input history

### 3. Exit Application

- Press `Start + Select` (or `Function + Start` on some devices)
- Or use `Ctrl+C` if running from command line

## Main Interface

### Event Display Area
- **Real-time Events**: Shows input events as they happen
- **Device Labels**: Each event shows which controller generated it
- **Timestamp**: Precise timing for each input (in milliseconds)
- **Event Type**: Button press/release, analog movement, device connect/disconnect

### Device Status Panel
- **Connected Devices**: List of all detected input devices
- **Device Type**: Built-in, USB, or Bluetooth connection
- **Button Scheme**: Current mapping (Knulli, PlayStation, Xbox)
- **Activity Indicator**: Shows which devices are actively sending input

### Event History
- **Scrollable Log**: Review past input events
- **Search/Filter**: Find specific events or devices
- **Export**: Save history to file for analysis
- **Clear**: Reset history buffer

## Configuration

### Basic Configuration File

Edit `/userdata/system/configs/input-tester/config.ini`:

```ini
[input-tester]
# Display settings
display_mode=windowed
refresh_rate=60
max_history_events=1000
show_timestamps=true
show_device_names=true

# Input settings
button_scheme=knulli
auto_detect_devices=true
ignore_duplicate_events=true

# Performance settings
max_cpu_usage=10
memory_limit_mb=50
```

### Button Schemes

Available button labeling schemes:

- **knulli**: Default Knulli labeling (A/B/X/Y cardinal positions)
- **playstation**: PlayStation symbols (△/○/✕/◻)
- **xbox**: Xbox labels (A/B/X/Y)

Change scheme in config or via hotkey `Function + Y`.

## Advanced Features

### Multiple Controller Testing

1. **Connect Multiple Devices**: Connect up to 4 controllers simultaneously
2. **Device Identification**: Each controller gets unique ID and color coding
3. **Simultaneous Testing**: Press buttons on different controllers to verify independence
4. **Conflict Detection**: App highlights if multiple controllers send conflicting input

### Input Latency Testing

1. **Enable Latency Mode**: Press `Function + L1`
2. **Visual Indicator**: On-screen timer shows input-to-display latency
3. **Performance Metrics**: View real-time performance statistics
4. **Benchmark**: Run automated latency tests

### Button Mapping Verification

1. **Select Scheme**: Choose target button labeling scheme
2. **Test Buttons**: Press each button to verify mapping
3. **Visual Feedback**: Correct mappings show green, incorrect show red
4. **Reference Guide**: On-screen reference shows expected button positions

### Event Logging and Debugging

1. **Enable Logging**: Set `log_to_file=true` in config
2. **Log Location**: `/userdata/system/logs/input-tester.log`
3. **Debug Mode**: Press `Function + R1` for detailed debugging
4. **Export Data**: Save event history as JSON or CSV

## Troubleshooting

### Common Issues

**No devices detected:**
```bash
# Check input devices
ls -la /dev/input/

# Check permissions
sudo chmod 666 /dev/input/event*

# Restart udev service
sudo systemctl restart udev
```

**High latency:**
- Reduce `refresh_rate` in config
- Close other applications
- Check system resources with `top`
- Try wired connection instead of Bluetooth

**Controller not working:**
- Verify controller compatibility
- Check connection type (USB/Bluetooth)
- Try different USB port
- Update controller firmware

**Application crashes:**
- Check log files for errors
- Verify sufficient memory available
- Update to latest version
- Report issue with system information

### Performance Optimization

For best performance:

1. **Close unnecessary applications** to free CPU/memory
2. **Use wired connections** for lower latency
3. **Adjust refresh rate** based on display capabilities
4. **Limit history size** if memory is constrained
5. **Disable visual effects** in configuration

### System Integration

**Add to Knulli Applications Menu:**
```bash
# Create desktop entry
cp /usr/share/applications/input-tester.desktop ~/.local/share/applications/

# Update application database
update-desktop-database ~/.local/share/applications/
```

**Enable Auto-start:**
```ini
# Add to config.ini
[input-tester]
auto_start=true
```

**Configure Hotkey:**
```ini
# Custom hotkey combination
[input-tester]
hotkey_combo=Function+Select
```

## Keyboard Shortcuts

| Shortcut | Function |
|----------|----------|
| `Start + Select` | Exit application |
| `Function + Start` | Toggle overlay mode |
| `Function + Y` | Cycle button schemes |
| `Function + L1` | Toggle latency display |
| `Function + R1` | Toggle debug mode |
| `Function + X` | Clear event history |
| `Function + B` | Export event log |
| `Ctrl + C` | Force exit (command line) |

## File Locations

```
/userdata/system/configs/input-tester/
├── config.ini              # Main configuration
├── devices/                # Per-device settings
└── profiles/               # Input profiles

/userdata/system/logs/
└── input-tester.log        # Application logs

/tmp/
└── input-tester-*          # Temporary files
```

## Support and Community

- **Documentation**: https://knulli.org/
- **Discord Community**: https://discord.gg/HXPS3DAeeB
- **Bug Reports**: https://github.com/knulli-cfw/input-tester/issues
- **Feature Requests**: https://github.com/knulli-cfw/input-tester/discussions

## Version Information

Check your version:
```bash
input-tester --version
```

Update to latest version:
```bash
pacman -Syu input-tester
```

## System Requirements

- **OS**: Knulli Gladiator II or later
- **Architecture**: ARM64 or x86_64
- **Memory**: Minimum 10MB free, recommended 50MB
- **Storage**: 5MB for application, 10MB for logs
- **Display**: Any supported resolution (320x240 minimum)
- **Input**: Any compatible game controller or built-in controls

## Legal and License

Knulli Input Tester is released under the MIT License. See included LICENSE file for details.

## Changelog

See `/usr/share/doc/input-tester/CHANGELOG.md` for version history and updates.
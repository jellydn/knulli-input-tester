# Knulli Input Tester - Device Deployment Guide

Comprehensive guide for building, packaging, and deploying to Trim UI Smart Pro (Knulli OS).

## Prerequisites

### Development Machine (macOS/Linux)
- CMake 3.20+
- C++ compiler (GCC 7+ or Clang 5+)
- SSH access to your Knulli device
- SCP or similar file transfer tool

### Trim UI Device
- Trim UI Smart Pro with Knulli OS installed
- SSH enabled
- Sufficient storage space (~20MB)
- Know your device's IP address on the network

## Step 1: Find Your Device's IP Address

### Method 1: From Knulli UI
1. Power on your Trim UI
2. Go to **System Settings** → **Network**
3. Note the IP address (e.g., `192.168.1.100`)

### Method 2: From Router
Check your router's connected devices list and find the Trim UI

### Method 3: Network Scan
```bash
nmap -p 22 192.168.1.0/24  # Replace with your subnet
```

Once you have the IP, test SSH connection:
```bash
ssh root@<DEVICE_IP>
# Default password: root (change after first login!)
```

## Step 2: Build the Application

### Option A: Build on Your Development Machine

**For macOS/Linux (native development):**
```bash
# Clean build
make distclean

# Build release version
make build

# Verify binary
file build/bin/knulli-input-tester
```

The binary will be at `build/bin/knulli-input-tester`.

### Option B: Build on the Device (if you prefer)

**If Trim UI has build tools installed:**
```bash
ssh root@<DEVICE_IP> << 'EOF'
cd /tmp
wget https://github.com/jellydn/knulli-input-tester/archive/main.zip
unzip main.zip
cd knulli-input-tester-main
make build
EOF
```

## Step 3: Transfer Binary to Device

### Method 1: Using SCP (Recommended)

```bash
# Copy to device
scp build/bin/knulli-input-tester root@<DEVICE_IP>:/tmp/

# Verify transfer
ssh root@<DEVICE_IP> ls -lh /tmp/knulli-input-tester
```

### Method 2: Using rsync (Faster for large projects)

```bash
rsync -avz build/bin/knulli-input-tester root@<DEVICE_IP>:/tmp/
```

### Method 3: Mount Network Share

```bash
# On macOS
mount_smbfs -o soft,nolocks //root:root@<DEVICE_IP>/mnt /Volumes/knulli

# Copy file
cp build/bin/knulli-input-tester /Volumes/knulli/
```

## Step 4: Install on Device

```bash
ssh root@<DEVICE_IP> << 'EOF'
# Copy to system bin directory
cp /tmp/knulli-input-tester /usr/bin/

# Make executable
chmod +x /usr/bin/knulli-input-tester

# Verify installation
ls -lh /usr/bin/knulli-input-tester
knulli-input-tester --version || echo "Binary ready"
EOF
```

## Step 5: Create Configuration

```bash
ssh root@<DEVICE_IP> << 'EOF'
# Create config directory
mkdir -p /userdata/system/configs/input-tester

# Create config file
cat > /userdata/system/configs/input-tester/config.ini << 'CONFIG'
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
CONFIG

echo "Config created at /userdata/system/configs/input-tester/config.ini"
EOF
```

## Step 6: Test the Application

### Quick Test - Run Directly

```bash
ssh root@<DEVICE_IP> '/usr/bin/knulli-input-tester'
```

Press buttons on your controller and watch for events in the display.

To exit, press **Function + Power** or press **Q**.

### Automated Test

```bash
ssh root@<DEVICE_IP> << 'EOF'
# Run with timeout
timeout 10 /usr/bin/knulli-input-tester &
sleep 2

# Test with input simulation (if uinput available)
echo "Testing input detection..."

# Check if app is responsive
pgrep -f knulli-input-tester > /dev/null && echo "✓ App running" || echo "✗ App failed"
EOF
```

## Step 7: Set Up as Systemd Service (Optional)

For background monitoring:

```bash
ssh root@<DEVICE_IP> << 'EOF'
# Copy service file
cat > /etc/systemd/system/knulli-input-tester.service << 'SERVICE'
[Unit]
Description=Knulli Input Tester
After=network.target

[Service]
Type=simple
User=root
ExecStart=/usr/bin/knulli-input-tester
Restart=on-failure
RestartSec=5

[Install]
WantedBy=multi-user.target
SERVICE

# Enable and start service
systemctl daemon-reload
systemctl enable knulli-input-tester
systemctl start knulli-input-tester

# Check status
systemctl status knulli-input-tester
EOF
```

View logs:
```bash
ssh root@<DEVICE_IP> journalctl -u knulli-input-tester -f
```

## Step 8: Integrate with EmulationStation (Optional)

Add to Knulli menu:

```bash
ssh root@<DEVICE_IP> << 'EOF'
# Create ES menu entry
mkdir -p /userdata/system/applications
cat > /userdata/system/applications/knulli-input-tester.sh << 'LAUNCHER'
#!/bin/bash
/usr/bin/knulli-input-tester
LAUNCHER

chmod +x /userdata/system/applications/knulli-input-tester.sh

# Restart EmulationStation to pick up new app
pkill emulationstation
EOF
```

The app will appear in **Main Menu** → **Applications** → **Tools**.

## Usage on Device

### Starting the App

**From terminal:**
```bash
ssh root@<DEVICE_IP>
knulli-input-tester
```

**From EmulationStation:**
Navigate to **Applications** → **Tools** → **Input Tester**

**As background service:**
```bash
ssh root@<DEVICE_IP>
systemctl start knulli-input-tester
journalctl -u knulli-input-tester -f
```

### Controls on Device

| Action | Button |
|--------|--------|
| Switch button scheme | Function + Y |
| Toggle history | Function + X |
| Clear history | Function + Select |
| Export history | Function + Start |
| Exit | Function + Power |

### Testing Input

1. Launch the app
2. Press buttons on your controller
3. Watch real-time events appear on screen
4. Verify latency is under 100ms
5. Check all buttons/axes appear correctly

## Troubleshooting

### Binary Won't Run

```bash
ssh root@<DEVICE_IP> << 'EOF'
# Check if it's executable
file /usr/bin/knulli-input-tester

# If permission denied
chmod +x /usr/bin/knulli-input-tester

# Try running with strace to see errors
strace /usr/bin/knulli-input-tester
EOF
```

### No Input Events Appearing

```bash
ssh root@<DEVICE_IP> << 'EOF'
# Check input devices
ls -la /dev/input/

# Check if input group exists
cat /etc/group | grep input

# Add root to input group (if needed)
usermod -a -G input root

# Verify permissions
ls -la /dev/input/event*
EOF
```

### Slow Performance / High Latency

```bash
ssh root@<DEVICE_IP> << 'EOF'
# Check CPU usage
top -b -n 1 | head -20

# Check memory
free -h

# Check running processes
ps aux | grep -v grep | wc -l

# Kill unnecessary services
systemctl stop bluetooth  # if not needed
systemctl stop avahi-daemon  # if not needed
EOF
```

### SSH Connection Issues

```bash
# Enable SSH on Knulli device (if not already enabled)
# Go to System Settings → Network → SSH → Enable

# Test connection
ssh -v root@<DEVICE_IP>

# If slow, check network
ping <DEVICE_IP>

# If connection refused, verify SSH is running on device
ssh root@<DEVICE_IP> systemctl status sshd
```

## Deployment Script (All-in-One)

Save as `deploy.sh`:

```bash
#!/bin/bash
set -e

DEVICE_IP=${1:-192.168.1.100}
DEVICE_USER=${2:-root}
BINARY_PATH="build/bin/knulli-input-tester"

echo "🎮 Knulli Input Tester - Device Deployment"
echo "Target: $DEVICE_USER@$DEVICE_IP"
echo ""

# Build
echo "📦 Building application..."
make clean
make build
echo "✓ Build complete"

# Transfer
echo "🚀 Transferring binary..."
scp "$BINARY_PATH" "$DEVICE_USER@$DEVICE_IP":/tmp/
echo "✓ Transfer complete"

# Install
echo "🔧 Installing on device..."
ssh "$DEVICE_USER@$DEVICE_IP" << 'EOF'
cp /tmp/knulli-input-tester /usr/bin/
chmod +x /usr/bin/knulli-input-tester
mkdir -p /userdata/system/configs/input-tester
echo "✓ Installation complete"
EOF

# Test
echo "🧪 Testing..."
ssh "$DEVICE_USER@$DEVICE_IP" timeout 5 /usr/bin/knulli-input-tester > /dev/null 2>&1 &
sleep 2
if ssh "$DEVICE_USER@$DEVICE_IP" pgrep -f knulli-input-tester > /dev/null; then
    echo "✓ Binary runs successfully"
    pkill -f knulli-input-tester || true
else
    echo "✗ Binary failed to run"
    exit 1
fi

echo ""
echo "✅ Deployment complete!"
echo ""
echo "Next steps:"
echo "  1. SSH to device: ssh $DEVICE_USER@$DEVICE_IP"
echo "  2. Run app: knulli-input-tester"
echo "  3. Press buttons to test"
echo "  4. Press Function+Power to exit"
```

Usage:
```bash
chmod +x deploy.sh
./deploy.sh 192.168.1.100 root
```

## Advanced: Cross-Compilation for ARM

If your device uses different CPU architecture (ARM vs x86):

```bash
# For ARM64 (Trim UI likely uses this)
cmake -DCMAKE_SYSTEM_NAME=Linux \
       -DCMAKE_SYSTEM_PROCESSOR=aarch64 \
       -DCMAKE_C_COMPILER=aarch64-linux-gnu-gcc \
       -DCMAKE_CXX_COMPILER=aarch64-linux-gnu-g++ \
       -DCMAKE_BUILD_TYPE=Release ..

make build
```

## Monitoring & Logging

### View Live Logs

```bash
ssh root@<DEVICE_IP> journalctl -u knulli-input-tester -f
```

### Check Performance

```bash
ssh root@<DEVICE_IP> << 'EOF'
# CPU and memory while running
watch -n 1 'ps aux | grep knulli-input-tester'

# Full system monitoring
top
EOF
```

### Export Logs

```bash
ssh root@<DEVICE_IP> journalctl -u knulli-input-tester > app.log
scp root@<DEVICE_IP>:app.log ./device-logs.log
```

## Uninstalling from Device

```bash
ssh root@<DEVICE_IP> << 'EOF'
# Stop service
systemctl stop knulli-input-tester 2>/dev/null || true

# Disable service
systemctl disable knulli-input-tester 2>/dev/null || true

# Remove files
rm -f /usr/bin/knulli-input-tester
rm -f /etc/systemd/system/knulli-input-tester.service
rm -rf /userdata/system/configs/input-tester

# Cleanup
systemctl daemon-reload

echo "✓ Uninstall complete"
EOF
```

## Next Steps

- Configure button mappings if needed
- Set up performance monitoring
- Create automated test suite
- Package as IPK/pacman for distribution

See [QUICKSTART.md](QUICKSTART.md) for usage instructions.

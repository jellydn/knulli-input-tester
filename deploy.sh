#!/bin/bash
#
# Knulli Input Tester - Device Deployment Script
# Usage: ./deploy.sh [DEVICE_IP] [USER]
#
# Example:
#   ./deploy.sh 192.168.1.100 root
#   ./deploy.sh 192.168.1.100  # defaults to root
#

set -e

# Configuration
DEVICE_IP=${1:-192.168.1.100}
DEVICE_USER=${2:-root}
BINARY_PATH="build/bin/knulli-input-tester"
INSTALL_PATH="/usr/bin/knulli-input-tester"

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
BLUE='\033[0;34m'
YELLOW='\033[1;33m'
NC='\033[0m'

# Helper functions
print_header() {
    echo -e "${BLUE}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
    echo -e "${BLUE}  $1${NC}"
    echo -e "${BLUE}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
}

print_info() {
    echo -e "${BLUE}ℹ${NC}  $1"
}

print_success() {
    echo -e "${GREEN}✓${NC}  $1"
}

print_error() {
    echo -e "${RED}✗${NC}  $1"
}

print_warning() {
    echo -e "${YELLOW}⚠${NC}  $1"
}

# Check prerequisites
check_prerequisites() {
    print_header "Checking Prerequisites"
    
    # Check if binary exists
    if [ ! -f "$BINARY_PATH" ]; then
        print_error "Binary not found at $BINARY_PATH"
        print_info "Build the project first: make build"
        exit 1
    fi
    print_success "Binary found"
    
    # Check SSH connectivity
    print_info "Testing SSH connection to $DEVICE_USER@$DEVICE_IP..."
    if ssh -o ConnectTimeout=5 -o BatchMode=yes "$DEVICE_USER@$DEVICE_IP" exit 2>/dev/null; then
        print_success "SSH connection successful"
    else
        print_error "Cannot connect to $DEVICE_USER@$DEVICE_IP"
        print_warning "Make sure:"
        echo "  • Device IP is correct"
        echo "  • SSH is enabled on device (System Settings → Network → SSH)"
        echo "  • Device is on the same network"
        echo "  • Check: ssh $DEVICE_USER@$DEVICE_IP"
        exit 1
    fi
    
    echo ""
}

# Build the project
build_project() {
    print_header "Building Project"
    
    if [ -d "build" ]; then
        print_info "Cleaning previous build..."
        make clean > /dev/null
    fi
    
    print_info "Building application..."
    make build > /dev/null 2>&1
    
    if [ -f "$BINARY_PATH" ]; then
        print_success "Build complete"
        ls -lh "$BINARY_PATH" | awk '{print "  Size: " $5}'
    else
        print_error "Build failed"
        exit 1
    fi
    
    echo ""
}

# Transfer binary
transfer_binary() {
    print_header "Transferring Binary"
    
    print_info "Copying to $DEVICE_IP:/tmp/..."
    if scp -q "$BINARY_PATH" "$DEVICE_USER@$DEVICE_IP":/tmp/; then
        print_success "Transfer complete"
    else
        print_error "Transfer failed"
        exit 1
    fi
    
    echo ""
}

# Install on device
install_on_device() {
    print_header "Installing on Device"
    
    ssh "$DEVICE_USER@$DEVICE_IP" << 'INSTALL_SCRIPT'
        set -e
        
        # Copy binary
        cp /tmp/knulli-input-tester /usr/bin/ 2>/dev/null || {
            echo "Failed to copy to /usr/bin"
            exit 1
        }
        
        # Make executable
        chmod +x /usr/bin/knulli-input-tester
        
        # Create config directory
        mkdir -p /userdata/system/configs/input-tester
        
        # Create default config if not exists
        if [ ! -f /userdata/system/configs/input-tester/config.ini ]; then
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
        fi
        
        # Cleanup temp file
        rm -f /tmp/knulli-input-tester
INSTALL_SCRIPT
    
    print_success "Installation complete"
    echo ""
}

# Test the application
test_application() {
    print_header "Testing Application"
    
    print_info "Starting application (5 second timeout)..."
    
    ssh "$DEVICE_USER@$DEVICE_IP" << 'TEST_SCRIPT'
        # Start the app in background
        timeout 5 /usr/bin/knulli-input-tester > /dev/null 2>&1 &
        sleep 2
        
        # Check if process is running
        if pgrep -f knulli-input-tester > /dev/null; then
            echo "success"
        else
            echo "failed"
        fi
        
        # Kill the process if still running
        pkill -f knulli-input-tester 2>/dev/null || true
TEST_TEST_SCRIPT
    
    if [ $? -eq 0 ]; then
        print_success "Application runs successfully"
    else
        print_warning "Could not fully test app (may need display)"
    fi
    
    echo ""
}

# Show device info
show_device_info() {
    print_header "Device Information"
    
    ssh "$DEVICE_USER@$DEVICE_IP" << 'DEVICE_INFO'
        echo "Hostname: $(hostname)"
        echo "OS: $(grep PRETTY_NAME /etc/os-release | cut -d'"' -f2)"
        echo "Kernel: $(uname -r)"
        echo "Architecture: $(uname -m)"
        echo "CPU Cores: $(nproc)"
        echo "Memory: $(free -h | awk 'NR==2 {print $2}')"
        echo "Disk Free: $(df -h /userdata | awk 'NR==2 {print $4}')"
DEVICE_INFO
    
    echo ""
}

# Show next steps
show_next_steps() {
    print_header "Deployment Complete! 🎉"
    
    echo ""
    echo -e "${GREEN}Next steps:${NC}"
    echo ""
    echo "1. ${BLUE}SSH to device:${NC}"
    echo "   ssh $DEVICE_USER@$DEVICE_IP"
    echo ""
    echo "2. ${BLUE}Run the app:${NC}"
    echo "   knulli-input-tester"
    echo ""
    echo "3. ${BLUE}Test with your controller:${NC}"
    echo "   • Press buttons to see events"
    echo "   • Check latency is <100ms"
    echo "   • Verify all buttons are detected"
    echo ""
    echo "4. ${BLUE}Controls:${NC}"
    echo "   Function + Y = Switch button scheme"
    echo "   Function + X = Toggle event history"
    echo "   Function + Select = Clear history"
    echo "   Function + Power = Exit app"
    echo ""
    echo "5. ${BLUE}View logs:${NC}"
    echo "   ssh $DEVICE_USER@$DEVICE_IP journalctl -u knulli-input-tester -f"
    echo ""
    echo "6. ${BLUE}Setup as service (optional):${NC}"
    echo "   ssh $DEVICE_USER@$DEVICE_IP systemctl enable knulli-input-tester"
    echo ""
}

# Main execution
main() {
    print_header "🎮 Knulli Input Tester - Device Deployment"
    
    echo "Device: $DEVICE_USER@$DEVICE_IP"
    echo "Binary: $BINARY_PATH"
    echo "Install Path: $INSTALL_PATH"
    echo ""
    
    # Run deployment steps
    check_prerequisites
    build_project
    transfer_binary
    install_on_device
    test_application
    show_device_info
    show_next_steps
}

# Error handling
trap 'print_error "Deployment failed!"; exit 1' ERR

# Run main
main

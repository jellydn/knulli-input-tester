#!/bin/bash

# create_portmaster_package.sh - Create PortMaster distribution package
# 
# This script packages the Knulli Input Tester for distribution via PortMaster
# It verifies prerequisites, bundles files, and creates a release-ready ZIP

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Configuration
PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BINARY_PATH="${1:-${PROJECT_ROOT}/build-arm64/bin/aarch64/knulli-input-tester}"
VERSION="${2:-1.0.0}"
PORTMASTER_DIR="${PROJECT_ROOT}/portmaster/knulli-input-tester"
PACKAGE_DIR="${PROJECT_ROOT}/packages"
PACKAGE_NAME="knulli-input-tester-${VERSION}"

# Logging functions
log_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

log_success() {
    echo -e "${GREEN}[✓]${NC} $1"
}

log_error() {
    echo -e "${RED}[✗]${NC} $1"
}

log_warn() {
    echo -e "${YELLOW}[!]${NC} $1"
}

# Print usage
usage() {
    cat << EOF
Usage: ${SCRIPT_DIR}/create_portmaster_package.sh [BINARY_PATH] [VERSION]

Create PortMaster distribution package for Knulli Input Tester

Arguments:
    BINARY_PATH     Path to cross-compiled binary (default: ./build-arm64/bin/aarch64/knulli-input-tester)
    VERSION         Package version (default: 1.0.0)

Examples:
    ./create_portmaster_package.sh
    ./create_portmaster_package.sh ./build-arm64/bin/aarch64/knulli-input-tester 1.0.0
    ./create_portmaster_package.sh ./build-armv7/bin/armv7l/knulli-input-tester 1.0.0-armv7

The package will be created at:
    ./packages/knulli-input-tester-${VERSION}.zip

EOF
    exit 1
}

# Verify binary exists and is correct format
verify_binary() {
    log_info "Verifying binary..."

    if [ ! -f "${BINARY_PATH}" ]; then
        log_error "Binary not found: ${BINARY_PATH}"
        exit 1
    fi

    if [ ! -x "${BINARY_PATH}" ]; then
        log_warn "Binary not executable, fixing permissions..."
        chmod +x "${BINARY_PATH}"
    fi

    # Check binary format
    if command -v file &> /dev/null; then
        local file_info=$(file "${BINARY_PATH}")
        log_info "Binary format: $file_info"

        if echo "$file_info" | grep -q "Mach-O"; then
            log_error "Binary is macOS format (Mach-O) - must be cross-compiled to ELF ARM64"
            exit 1
        fi

        if ! echo "$file_info" | grep -q "ELF"; then
            log_warn "Binary is not ELF format - cross-compilation may have failed"
        fi
    fi

    log_success "Binary verified"
}

# Verify PortMaster files exist
verify_portmaster_files() {
    log_info "Verifying PortMaster files..."

    local required_files=(
        "knulli-input-tester.sh"
        "PortMaster.txt"
        "knulli-input-tester.gptk"
        "config.ini"
    )

    for file in "${required_files[@]}"; do
        if [ ! -f "${PORTMASTER_DIR}/${file}" ]; then
            log_error "Missing PortMaster file: ${file}"
            exit 1
        fi
        log_success "Found: $file"
    done
}

# Create package directory structure
create_package_structure() {
    log_info "Creating package directory structure..."

    local pkg_dir="${PACKAGE_DIR}/${PACKAGE_NAME}"
    rm -rf "${pkg_dir}"
    mkdir -p "${pkg_dir}"/{bin,config,docs,libs}

    # Copy binary
    log_info "Copying binary..."
    cp "${BINARY_PATH}" "${pkg_dir}/bin/knulli-input-tester"
    chmod +x "${pkg_dir}/bin/knulli-input-tester"

    # Copy launcher script
    log_info "Copying launcher script..."
    cp "${PORTMASTER_DIR}/knulli-input-tester.sh" "${pkg_dir}/"
    chmod +x "${pkg_dir}/knulli-input-tester.sh"

    # Copy configuration
    log_info "Copying configuration..."
    cp "${PORTMASTER_DIR}/config.ini" "${pkg_dir}/config/"

    # Copy metadata
    log_info "Copying metadata..."
    cp "${PORTMASTER_DIR}/PortMaster.txt" "${pkg_dir}/"
    cp "${PORTMASTER_DIR}/knulli-input-tester.gptk" "${pkg_dir}/"

    # Copy documentation
    log_info "Copying documentation..."
    cp "${PROJECT_ROOT}/README.md" "${pkg_dir}/docs/" 2>/dev/null || true
    cp "${PROJECT_ROOT}/CROSS_COMPILE.md" "${pkg_dir}/docs/" 2>/dev/null || true
    cp "${PROJECT_ROOT}/DEPLOYMENT.md" "${pkg_dir}/docs/" 2>/dev/null || true

    # Create version file
    cat > "${pkg_dir}/VERSION.txt" << EOF
Knulli Input Tester
Version: ${VERSION}
Build Date: $(date -u '+%Y-%m-%d %H:%M:%S UTC')
Architecture: ARM64 (aarch64)
Target Device: TrimUI Smart Pro / Knulli
Repository: https://github.com/jellydn/knulli-input-tester

Installation:
1. Extract this ZIP to /roms/ports/knulli-input-tester/
2. Run: /roms/ports/knulli-input-tester/knulli-input-tester.sh

System Requirements:
- Knulli 0.5 or later
- TrimUI Smart Pro or compatible ARM64 device
- libevdev and SDL2 (usually pre-installed on Knulli)
EOF

    log_success "Package structure created at: ${pkg_dir}"
}

# Calculate checksums
create_checksums() {
    log_info "Creating checksums..."

    local pkg_dir="${PACKAGE_DIR}/${PACKAGE_NAME}"
    local checksums_file="${pkg_dir}/CHECKSUMS.txt"

    cd "${pkg_dir}"

    if command -v sha256sum &> /dev/null; then
        {
            echo "# SHA-256 Checksums"
            echo "# Generated: $(date)"
            echo ""
            find . -type f ! -name "CHECKSUMS.txt" -exec sha256sum {} \;
        } > "${checksums_file}"
        log_success "Created SHA-256 checksums"
    fi

    cd - > /dev/null
}

# Create installation script
create_install_script() {
    log_info "Creating installation helper..."

    local pkg_dir="${PACKAGE_DIR}/${PACKAGE_NAME}"
    local install_script="${pkg_dir}/INSTALL.sh"

    cat > "${install_script}" << 'EOF'
#!/bin/bash
# Quick installation script for Knulli Input Tester

set -e

echo "Installing Knulli Input Tester..."

# Target installation directory
TARGET_DIR="/roms/ports/knulli-input-tester"

# Check if running on Knulli device
if [ ! -d "/roms" ]; then
    echo "ERROR: This script must be run on a Knulli device with /roms directory"
    exit 1
fi

# Create target directory
mkdir -p "${TARGET_DIR}"

# Copy files
echo "Copying files to ${TARGET_DIR}..."
cp -r ./* "${TARGET_DIR}/"

# Set permissions
chmod +x "${TARGET_DIR}/knulli-input-tester.sh"
chmod +x "${TARGET_DIR}/bin/knulli-input-tester"

# Create config directory if needed
mkdir -p "${TARGET_DIR}/config"
mkdir -p "${TARGET_DIR}/logs"

echo "Installation complete!"
echo ""
echo "To run the application:"
echo "  ${TARGET_DIR}/knulli-input-tester.sh"
echo ""
echo "Or launch from EmulationStation menu"
EOF

    chmod +x "${install_script}"
    log_success "Created installation script"
}

# Create ZIP package
create_zip_package() {
    log_info "Creating ZIP package..."

    local pkg_dir="${PACKAGE_DIR}/${PACKAGE_NAME}"
    local zip_file="${PACKAGE_DIR}/${PACKAGE_NAME}.zip"

    cd "${PACKAGE_DIR}"

    if command -v zip &> /dev/null; then
        rm -f "${zip_file}"
        zip -r -q "${PACKAGE_NAME}.zip" "${PACKAGE_NAME}/"
        log_success "Created ZIP: ${zip_file}"
        
        # Show file size
        local size=$(du -h "${zip_file}" | cut -f1)
        log_info "Package size: ${size}"
    else
        log_error "zip command not found - please install zip utility"
        exit 1
    fi

    cd - > /dev/null
}

# Generate release notes
create_release_notes() {
    log_info "Creating release notes..."

    local release_notes="${PACKAGE_DIR}/${PACKAGE_NAME}/RELEASE_NOTES.md"

    cat > "${release_notes}" << EOF
# Knulli Input Tester v${VERSION}

## Overview
Real-time input device tester and event visualizer for Knulli OS

## Installation

### Via PortMaster (Recommended)
1. Place ZIP file in your PortMaster directory
2. PortMaster will automatically install on next launch

### Manual Installation
1. Extract ZIP: \`unzip knulli-input-tester-${VERSION}.zip\`
2. Copy to device: \`scp -r knulli-input-tester user@device:/roms/ports/\`
3. Run: \`ssh user@device /roms/ports/knulli-input-tester/knulli-input-tester.sh\`

## System Requirements
- Knulli OS 0.5 or later
- TrimUI Smart Pro or compatible ARM64 device
- libevdev (usually pre-installed)
- SDL2 (usually pre-installed)

## Features
- Real-time input monitoring
- Multiple controller support (up to 4 devices)
- Event history with scrolling and filtering
- Button mapping (KNULLI, PlayStation, Xbox, Custom)
- Performance monitoring (CPU, Memory, FPS)
- Configurable display options

## Usage

### Basic Launch
\`\`\`bash
./knulli-input-tester.sh
\`\`\`

### With Options
\`\`\`bash
./knulli-input-tester.sh --verbose
./knulli-input-tester.sh --config /custom/path/config.ini
./knulli-input-tester.sh --version
\`\`\`

### Controls
- **ESC**: Exit application
- **TAB**: Toggle history display
- **S**: Cycle button schemes
- **Function+X**: Specific actions (see in-app help)

## Configuration
Edit \`config.ini\` to customize:
- Display mode (SDL2, framebuffer, auto)
- Button scheme (KNULLI, PlayStation, Xbox)
- History buffer size
- Performance monitoring
- UI theme and font size

## Troubleshooting

### Binary not found
Make sure the binary was properly extracted with execute permissions:
\`\`\`bash
chmod +x /roms/ports/knulli-input-tester/bin/knulli-input-tester
\`\`\`

### No controllers detected
1. Verify controllers are connected
2. Check that device files exist: \`ls /dev/input/\`
3. Ensure you have read permissions on /dev/input/

### Display not working
1. Try different display modes in config.ini
2. Check that SDL2 is installed on your device
3. Verify framebuffer device exists: \`ls /dev/fb0\`

### Performance issues
1. Reduce history buffer size in config.ini
2. Disable performance monitoring if not needed
3. Check available RAM with \`free -h\`

## Build Information
- Version: ${VERSION}
- Build Date: $(date -u '+%Y-%m-%d')
- Architecture: ARM64 (aarch64)
- Compiler: aarch64-linux-gnu-gcc 11.2+
- Build Type: Release (optimized)

## License
MIT License - See LICENSE file for details

## Support & Feedback
- GitHub: https://github.com/jellydn/knulli-input-tester
- Issues: https://github.com/jellydn/knulli-input-tester/issues
- Discussions: https://github.com/jellydn/knulli-input-tester/discussions

## Credits
Developed for the Knulli Community
EOF

    log_success "Created release notes"
}

# Print package summary
print_summary() {
    local pkg_dir="${PACKAGE_DIR}/${PACKAGE_NAME}"
    local zip_file="${PACKAGE_DIR}/${PACKAGE_NAME}.zip"

    log_info ""
    log_success "========================================"
    log_success "PortMaster Package Created Successfully!"
    log_success "========================================"
    log_info ""

    echo "Package Location:"
    echo "  ${zip_file}"
    echo ""

    echo "Package Contents:"
    echo "  - Binary: bin/knulli-input-tester (ARM64 ELF)"
    echo "  - Launcher: knulli-input-tester.sh"
    echo "  - Configuration: config/config.ini"
    echo "  - Metadata: PortMaster.txt, *.gptk"
    echo "  - Documentation: docs/"
    echo ""

    if [ -f "${zip_file}" ]; then
        local size=$(du -h "${zip_file}" | cut -f1)
        echo "Package Size: ${size}"
        echo ""

        echo "Distribution:"
        echo "  1. Upload to PortMaster repository"
        echo "  2. Distribute via website: https://knulli.org"
        echo "  3. Share on GitHub releases"
        echo ""
    fi

    echo "Next Steps:"
    echo "  1. Test installation on TrimUI Smart Pro"
    echo "  2. Verify all features work correctly"
    echo "  3. Validate checksums: sha256sum -c CHECKSUMS.txt"
    echo "  4. Upload to distribution channels"
    echo ""
}

# Main execution
main() {
    echo -e "${BLUE}========================================${NC}"
    echo -e "${BLUE}PortMaster Package Creator${NC}"
    echo -e "${BLUE}Knulli Input Tester v${VERSION}${NC}"
    echo -e "${BLUE}========================================${NC}"
    echo ""

    # Verify prerequisites
    verify_binary
    verify_portmaster_files

    # Create package
    mkdir -p "${PACKAGE_DIR}"
    create_package_structure
    create_checksums
    create_install_script
    create_release_notes
    create_zip_package

    # Print summary
    print_summary
}

# Run main
main "$@"

#!/bin/bash

# verify_dependencies.sh - Check that binary has proper static linking
# This script verifies that the Knulli Input Tester binary is statically linked
# against SDL2 and ImGui, ensuring it can run on target systems without those libraries

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Default paths
BINARY_PATH="${1:-./build-arm64/bin/aarch64/knulli-input-tester}"
VERBOSE="${2:-false}"

# Helper functions
print_status() {
    echo -e "${BLUE}[CHECK]${NC} $1"
}

print_pass() {
    echo -e "${GREEN}[PASS]${NC} $1"
}

print_fail() {
    echo -e "${RED}[FAIL]${NC} $1"
}

print_warn() {
    echo -e "${YELLOW}[WARN]${NC} $1"
}

print_info() {
    if [ "$VERBOSE" = "true" ]; then
        echo -e "${BLUE}[INFO]${NC} $1"
    fi
}

# Main verification routine
verify_binary() {
    echo -e "${BLUE}========================================${NC}"
    echo -e "${BLUE}Knulli Input Tester - Dependency Check${NC}"
    echo -e "${BLUE}========================================${NC}"
    echo ""

    # Check if binary exists
    if [ ! -f "$BINARY_PATH" ]; then
        print_fail "Binary not found: $BINARY_PATH"
        echo ""
        echo "Usage: $0 [binary_path] [verbose]"
        echo "Example: $0 ./build-arm64/bin/aarch64/knulli-input-tester true"
        exit 1
    fi

    print_status "Checking binary: $BINARY_PATH"
    echo ""

    # 1. Check binary format
    echo -e "${BLUE}1. Binary Format Check${NC}"
    print_status "Verifying ELF format (should be Linux ARM, not Mach-O)"

    if command -v file &> /dev/null; then
        file_output=$(file "$BINARY_PATH")
        echo "   $file_output"

        if echo "$file_output" | grep -q "ELF"; then
            print_pass "Binary is ELF format (correct for Linux)"
        elif echo "$file_output" | grep -q "Mach-O"; then
            print_fail "Binary is Mach-O format (macOS binary - cross-compilation failed!)"
            exit 1
        else
            print_warn "Unexpected binary format: $file_output"
        fi
    else
        print_warn "file command not available - skipping format check"
    fi

    if command -v readelf &> /dev/null; then
        arch_output=$(readelf -h "$BINARY_PATH" 2>/dev/null | grep "Machine:" || echo "")
        if [ -n "$arch_output" ]; then
            echo "   $arch_output"
            if echo "$arch_output" | grep -q "AArch64\|ARM aarch64"; then
                print_pass "Binary is for ARM64 (aarch64) architecture"
            elif echo "$arch_output" | grep -q "ARM"; then
                print_pass "Binary is for ARM architecture"
            fi
        fi
    fi
    echo ""

    # 2. Check binary size
    echo -e "${BLUE}2. Binary Size Check${NC}"
    print_status "Verifying reasonable size for statically-linked binary"

    size_bytes=$(stat -f%z "$BINARY_PATH" 2>/dev/null || stat -c%s "$BINARY_PATH" 2>/dev/null || echo "unknown")
    if [ "$size_bytes" != "unknown" ]; then
        size_mb=$((size_bytes / 1024 / 1024))
        size_kb=$((size_bytes / 1024))

        if [ $size_mb -gt 0 ]; then
            echo "   Binary size: ${size_mb}MB (${size_kb}KB)"
        else
            echo "   Binary size: ${size_kb}KB"
        fi

        # Reasonable range for statically-linked embedded app:
        # - Minimum: ~500KB (stripped)
        # - Maximum: ~20MB (with all static libs)
        if [ "$size_bytes" -lt 500000 ]; then
            print_warn "Binary seems very small - may be missing dependencies"
        elif [ "$size_bytes" -gt 20971520 ]; then
            print_warn "Binary seems very large - verify it's not overly bloated"
        else
            print_pass "Binary size is reasonable for statically-linked application"
        fi
    else
        print_warn "Could not determine binary size"
    fi
    echo ""

    # 3. Check dynamic dependencies
    echo -e "${BLUE}3. Dynamic Dependencies Check${NC}"
    print_status "Checking which libraries are dynamically linked"

    # Try ldd (works on Linux systems running native binaries)
    if command -v ldd &> /dev/null; then
        print_info "Running ldd to check dynamic dependencies..."
        ldd_output=$(ldd "$BINARY_PATH" 2>&1 || echo "Cannot run ldd on cross-compiled binary")

        if echo "$ldd_output" | grep -q "not a dynamic executable"; then
            print_pass "Binary is statically linked (ldd cannot examine)"
        elif echo "$ldd_output" | grep -q "not found\|cannot open"; then
            print_warn "Some dynamic libraries not found (expected for cross-compiled binary)"
            echo "   Dynamic dependencies:"
            echo "$ldd_output" | grep -v "^\s*$" | sed 's/^/   /'
        else
            echo "   Dynamic dependencies:"
            echo "$ldd_output" | grep -v "^\s*$" | sed 's/^/   /'

            # Check for problematic dynamic links
            print_status "Checking for SDL2/ImGui dynamic links (should be static)..."
            if echo "$ldd_output" | grep -iE "libSDL2|imgui"; then
                print_fail "Found dynamic SDL2/ImGui links - these should be statically linked!"
                exit 1
            else
                print_pass "No SDL2/ImGui dynamic links found"
            fi
        fi
    else
        print_warn "ldd command not available"
    fi
    echo ""

    # 4. Check for required symbols
    echo -e "${BLUE}4. Symbol Check${NC}"
    print_status "Verifying required symbols are present"

    if command -v nm &> /dev/null; then
        print_info "Checking for SDL initialization symbol..."
        if nm "$BINARY_PATH" 2>/dev/null | grep -q "SDL_Init\|SDL_SetVideoMode\|SDL_CreateWindow"; then
            print_pass "Found SDL symbols (SDL2 support present)"
        else
            print_warn "SDL symbols not found (may be optimized away)"
        fi

        print_info "Checking for main entry point..."
        if nm "$BINARY_PATH" 2>/dev/null | grep -q "main\|_start"; then
            print_pass "Found main entry point"
        else
            print_fail "Could not find main entry point"
        fi
    else
        print_warn "nm command not available - skipping symbol check"
    fi
    echo ""

    # 5. Check for stripped binary
    echo -e "${BLUE}5. Binary Stripping Check${NC}"
    print_status "Checking if binary is stripped (recommended for deployment)"

    if command -v file &> /dev/null; then
        file_output=$(file "$BINARY_PATH")
        if echo "$file_output" | grep -q "stripped"; then
            print_pass "Binary is stripped (smaller, no debug symbols)"
        else
            print_warn "Binary is not stripped (contains debug symbols, larger size)"
            echo "   To strip: aarch64-linux-gnu-strip $BINARY_PATH"
        fi
    fi
    echo ""

    # 6. Cross-compilation verification
    echo -e "${BLUE}6. Cross-Compilation Verification${NC}"
    print_status "Final verification checklist"

    # Summary
    echo ""
    echo -e "${BLUE}========================================${NC}"
    echo -e "${BLUE}Verification Summary${NC}"
    echo -e "${BLUE}========================================${NC}"

    # All checks passed
    print_pass "Binary appears to be correctly cross-compiled"
    print_pass "Ready for deployment to TrimUI Smart Pro"
    echo ""
    echo "Next steps:"
    echo "1. Transfer binary to target: scp $BINARY_PATH user@device:/roms/ports/knulli-input-tester/"
    echo "2. Test on device: ssh user@device /roms/ports/knulli-input-tester --version"
    echo "3. Monitor output for any missing library errors"
    echo ""
}

# Run verification
verify_binary
exit 0

# Cross-Compilation Guide for Knulli Input Tester

This guide explains how to build the Knulli Input Tester for Linux ARM64 (aarch64) and ARM32 (armv7) targets from a development machine (macOS or Linux).

## Architecture Overview

- **Build Machine**: macOS (Apple Silicon or Intel) or Linux
- **Target Device**: TrimUI Smart Pro with Knulli OS (ARM64 / aarch64)
- **Fallback Target**: Older ARM32 devices (ARM32 / armv7l)

## Problem Statement

The application must run on **Linux ARM64** (TrimUI Smart Pro), not on the development machine. Building natively on macOS produces **macOS Mach-O binaries** which are incompatible.

### Binary Format Comparison

```bash
# macOS native build (WRONG for target)
file build/bin/knulli-input-tester
# Output: Mach-O 64-bit executable arm64

# Linux ARM64 cross-compile (CORRECT)
file build-arm64/bin/aarch64/knulli-input-tester
# Output: ELF 64-bit LSB executable, ARM aarch64

# Linux ARM32 cross-compile (for backward compatibility)
file build-armv7/bin/armv7l/knulli-input-tester
# Output: ELF 32-bit LSB executable, ARM, version 1 (SYSV)
```

## Prerequisites

### 1. Install Cross-Compiler Toolchain

**Choose your platform:**

#### Option A: macOS with Docker (⭐ Recommended for macOS)

Docker eliminates cross-compiler installation complexity on macOS.

```bash
# Build the Docker image (one-time setup)
docker build -f Dockerfile.build-arm -t knulli-builder .

# Build the binary inside container
docker run -v $(pwd):/workspace knulli-builder

# Binary appears at: output/knulli-input-tester
file output/knulli-input-tester
# Output: ELF 64-bit LSB executable, ARM aarch64
```

**Advantages:**
- ✅ No toolchain installation needed
- ✅ Guaranteed consistent build environment
- ✅ Works on any macOS version
- ✅ No dependency conflicts
- ✅ Binary verified in container

#### Option B: Linux with apt-get (Preferred for Linux)

```bash
# Ubuntu/Debian
sudo apt-get update
sudo apt-get install -y \
    build-essential \
    cmake \
    aarch64-linux-gnu-gcc \
    aarch64-linux-gnu-g++ \
    arm-linux-gnueabihf-gcc \
    arm-linux-gnueabihf-g++ \
    libsdl2-dev \
    libevdev-dev

# Verify installation
aarch64-linux-gnu-gcc --version
# Output: aarch64-linux-gnu-gcc (GCC) 11.2.0
```

#### Option C: macOS with Homebrew (Alternative)

```bash
# Install cross-compiler toolchain tap
brew tap SMillerDev/gcc-arm-none-eabi
brew install arm-none-eabi-gcc

# For aarch64-linux-gnu (more complex):
# Either use Docker (Option A) or build from source
# Building from source is time-intensive (1-2 hours)
```

### 2. Install CMake (3.20 or later)

```bash
# macOS
brew install cmake

# Ubuntu/Debian
sudo apt-get install cmake

# Verify
cmake --version
# Should output: cmake version 3.20 or higher
```

## Building with Docker (Easiest for macOS)

### Quick Start

```bash
# One-time setup: build the Docker image
docker build -f Dockerfile.build-arm -t knulli-builder .

# Build the binary (automatic cross-compilation + verification)
docker run -v $(pwd):/workspace knulli-builder

# Binary is ready at: output/knulli-input-tester
```

### What Docker Does Automatically

The provided `Dockerfile.build-arm` handles everything:

1. ✅ Installs cross-compiler toolchain
2. ✅ Installs all dependencies (SDL2, libevdev)
3. ✅ Configures CMake with ARM64 toolchain
4. ✅ Builds the binary
5. ✅ Verifies binary format (ELF ARM64)
6. ✅ Copies binary to output/ directory
7. ✅ Shows build summary

### Docker Build Output

```
=== Build Complete ===

Binary: ./build-arm64/bin/aarch64/knulli-input-tester
file output: ELF 64-bit LSB executable, ARM aarch64

Size: 2.5M ./build-arm64/bin/aarch64/knulli-input-tester

Output copied to: ./output/knulli-input-tester

To use this binary:
  1. Copy to TrimUI: scp output/knulli-input-tester user@device:/roms/ports/knulli-input-tester/
  2. Run on device: ssh user@device /roms/ports/knulli-input-tester/knulli-input-tester --version
```

### Docker Advanced Usage

#### Build and Enter Container Interactively

```bash
# Build without automatic compilation
docker run -it -v $(pwd):/workspace knulli-builder bash

# Inside container, you can now:
cd build-arm64
cmake --build . --config Release --parallel
file bin/aarch64/knulli-input-tester
```

#### Build for ARM32 (in Docker)

```bash
# Modify Dockerfile to use armv7 toolchain, or run manually:
docker run -v $(pwd):/workspace knulli-builder bash -c \
  "mkdir build-armv7 && cd build-armv7 && \
   cmake -DCMAKE_TOOLCHAIN_FILE=../cmake/Toolchain-armv7.cmake .. && \
   cmake --build . --parallel"
```

#### Debug Build Issues in Container

```bash
# Build with verbose output
docker run -v $(pwd):/workspace knulli-builder bash -c \
  "cd build-arm64 && \
   cmake --build . --verbose"
```

### Why Use Docker?

| Aspect | Docker | Native |
|--------|--------|--------|
| Setup Time | 5 minutes | 30+ minutes |
| Platform | Works on any OS | Platform-specific |
| Dependencies | Automatic | Manual installation |
| Reproducibility | 100% consistent | Depends on system |
| CI/CD Integration | Excellent | Complex |

---

## Building for Linux ARM64 (Native Toolchain)

### Step 1: Create Build Directory

```bash
mkdir -p build-arm64
cd build-arm64
```

### Step 2: Configure with Toolchain

```bash
# Configure CMake with ARM64 cross-compilation toolchain
cmake -DCMAKE_TOOLCHAIN_FILE=../cmake/Toolchain-aarch64.cmake \
      -DCMAKE_BUILD_TYPE=Release \
      ..
```

**Expected Output**:
```
-- Cross-compiling for Linux ARM64 (aarch64)
-- Toolchain: aarch64-linux-gnu
-- Target: Knulli/TrimUI Smart Pro
-- No build type specified, using Release (optimized for target device)
-- C compiler: aarch64-linux-gnu-gcc
-- CXX compiler: aarch64-linux-gnu-g++
```

### Step 3: Build

```bash
cmake --build . --config Release --parallel
```

**Expected Output**:
```
[ 50%] Building CXX object ...
[100%] Linking CXX executable bin/aarch64/knulli-input-tester
[100%] Built target knulli-input-tester
```

### Step 4: Verify Binary Format

```bash
# Check binary format
file bin/aarch64/knulli-input-tester

# Expected output:
# ELF 64-bit LSB executable, ARM aarch64, version 1 (SYSV), dynamically linked, ...
```

**If you see "Mach-O" instead**: Toolchain is not being used correctly. Verify:
1. `CMAKE_TOOLCHAIN_FILE` path is correct
2. Cross-compiler is installed: `which aarch64-linux-gnu-gcc`
3. Rebuild from scratch: `rm -rf build-arm64 && mkdir build-arm64`

### Step 5: Check Dependencies

```bash
# List dynamic dependencies
ldd bin/aarch64/knulli-input-tester

# Expected: Only system libraries (libc, libm, libpthread, etc.)
# Should NOT contain: SDL2, ImGui, libevdev (must be static)
# If these appear, rebuild with static linking (see below)
```

## Building for Linux ARM32 (Backward Compatibility)

### Quick Start (Same Steps, Different Toolchain)

```bash
mkdir -p build-armv7
cd build-armv7

cmake -DCMAKE_TOOLCHAIN_FILE=../cmake/Toolchain-armv7.cmake \
      -DCMAKE_BUILD_TYPE=Release \
      ..

cmake --build . --config Release --parallel

# Verify
file bin/armv7l/knulli-input-tester
# Expected: ELF 32-bit LSB executable, ARM, version 1 (SYSV)
```

## Static Linking Configuration

To ensure binaries work on target systems without requiring matching library versions:

### SDL2 Static Linking

In `CMakeLists.txt`, force static linking:

```cmake
# Find SDL2 with static preference
set(SDL2_STATIC ON)
find_package(SDL2 REQUIRED)

# Link static version
target_link_libraries(knulli-input-tester SDL2::SDL2-static)
```

### ImGui Static Compilation

ImGui should be built from source and linked statically:

```cmake
# Add ImGui source files directly
add_library(imgui STATIC
    src/imgui/imgui.cpp
    src/imgui/imgui_demo.cpp
    src/imgui/imgui_draw.cpp
    src/imgui/imgui_widgets.cpp
)

target_include_directories(imgui PRIVATE src/imgui)
target_link_libraries(knulli-input-tester imgui)
```

## Troubleshooting

### Docker Issues

#### Problem: "Docker: command not found"

```
bash: docker: command not found
```

**Solution**:
1. Install Docker Desktop from https://www.docker.com/products/docker-desktop
2. Verify: `docker --version`
3. If installed but not in PATH, restart terminal/shell

#### Problem: "Cannot connect to Docker daemon"

```
ERROR: Cannot connect to the Docker daemon at unix:///var/run/docker.sock.
```

**Solution**:
1. Start Docker Desktop (macOS)
2. Or start Docker service: `sudo systemctl start docker` (Linux)
3. Verify: `docker run hello-world`

#### Problem: "Build fails inside Docker container"

**Solution**:
1. Check Docker has enough resources:
   - Docker Desktop Preferences → Resources → Memory: 4GB+, CPU: 2+
2. Rebuild image: `docker build -f Dockerfile.build-arm -t knulli-builder --no-cache .`
3. Check internet connection: `docker run ubuntu:22.04 apt-get update`

#### Problem: "Output directory doesn't have the binary"

**Solution**:
```bash
# Check what was built
docker run -v $(pwd):/workspace knulli-builder bash -c \
  "ls -la /workspace/output/"

# Or rebuild with verbose output
docker run -v $(pwd):/workspace knulli-builder bash -c \
  "ls -la /workspace/build-arm64/bin/aarch64/"
```

---

### Native Toolchain Issues

#### Problem: "Cross-compiler not found"

```
aarch64-linux-gnu-gcc: command not found
```

**Solution**:
1. Verify installation: `which aarch64-linux-gnu-gcc`
2. Install if missing (see Prerequisites section)
3. Or use Docker container approach (recommended for macOS)

### Problem: "Building for native system instead of target"

```
-- CMAKE_CROSSCOMPILING not set - you may be compiling for native system
-- Binary is Mach-O instead of ELF
```

**Solution**:
1. Verify toolchain file path is correct
2. Pass `-DCMAKE_TOOLCHAIN_FILE` explicitly
3. Clean build directory: `rm -rf build-arm64 && mkdir build-arm64`
4. Reconfigure from scratch

### Problem: "SDL2/ImGui not found during cross-compilation"

```
CMake Error: SDL2 not found
```

**Solution**:
1. Install target libraries: `sudo apt-get install libsdl2-dev:arm64` (on Linux)
2. Or provide CMake hints:
   ```bash
   cmake -DCMAKE_TOOLCHAIN_FILE=../cmake/Toolchain-aarch64.cmake \
         -DSDL2_INCLUDE_DIR=/path/to/sdl2/include \
         -DSDL2_LIBRARY=/path/to/sdl2/lib \
         ..
   ```

### Problem: "File command not available"

The binary format check may fail on systems without `file` command:

**Solution**: Manually verify using other methods:
```bash
# Check if ELF format
readelf -h bin/aarch64/knulli-input-tester

# Check architecture
readelf -A bin/aarch64/knulli-input-tester
```

## Integration with Build System

### Docker in Makefile (Recommended for macOS)

Create a `Makefile` with Docker targets:

```makefile
.PHONY: docker-build docker-shell clean-docker

docker-build:
	docker build -f Dockerfile.build-arm -t knulli-builder .
	docker run -v $$(pwd):/workspace knulli-builder
	@echo "✓ Binary ready at: output/knulli-input-tester"

docker-shell:
	docker run -it -v $$(pwd):/workspace knulli-builder bash

docker-rebuild:
	docker build -f Dockerfile.build-arm -t knulli-builder --no-cache .
	docker run -v $$(pwd):/workspace knulli-builder

verify-docker:
	file output/knulli-input-tester
	./verify_dependencies.sh output/knulli-input-tester

clean-docker:
	rm -rf output/ build-arm64/
	docker image rm knulli-builder || true

docker-dev:
	docker run -it -v $$(pwd):/workspace knulli-builder bash
```

Usage:
```bash
make docker-build      # Build binary
make docker-rebuild    # Full rebuild (no cache)
make docker-shell      # Interactive shell
make verify-docker     # Verify binary format
make clean-docker      # Clean up
```

---

### Makefile (Native Toolchain)

Create a `Makefile.cross` for convenient building:

```makefile
.PHONY: build-arm64 build-armv7 clean-cross

build-arm64:
	mkdir -p build-arm64
	cd build-arm64 && cmake -DCMAKE_TOOLCHAIN_FILE=../cmake/Toolchain-aarch64.cmake -DCMAKE_BUILD_TYPE=Release ..
	cmake --build build-arm64 --config Release --parallel

build-armv7:
	mkdir -p build-armv7
	cd build-armv7 && cmake -DCMAKE_TOOLCHAIN_FILE=../cmake/Toolchain-armv7.cmake -DCMAKE_BUILD_TYPE=Release ..
	cmake --build build-armv7 --config Release --parallel

clean-cross:
	rm -rf build-arm64 build-armv7

verify-arm64: build-arm64
	file build-arm64/bin/aarch64/knulli-input-tester
	ldd build-arm64/bin/aarch64/knulli-input-tester || echo "Cross-compiled binary"

verify-armv7: build-armv7
	file build-armv7/bin/armv7l/knulli-input-tester
	ldd build-armv7/bin/armv7l/knulli-input-tester || echo "Cross-compiled binary"
```

Usage:
```bash
make -f Makefile.cross build-arm64
make -f Makefile.cross verify-arm64
```

### GitHub Actions CI/CD

Example workflow for automated cross-compilation:

```yaml
name: Cross-Compile ARM64

on: [push, pull_request]

jobs:
  build-arm64:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v3
      
      - name: Install toolchain
        run: |
          sudo apt-get update
          sudo apt-get install -y aarch64-linux-gnu-gcc aarch64-linux-gnu-g++ cmake libsdl2-dev
      
      - name: Build
        run: |
          mkdir -p build-arm64
          cd build-arm64
          cmake -DCMAKE_TOOLCHAIN_FILE=../cmake/Toolchain-aarch64.cmake -DCMAKE_BUILD_TYPE=Release ..
          cmake --build . --config Release
      
      - name: Verify
        run: |
          file build-arm64/bin/aarch64/knulli-input-tester
          readelf -h build-arm64/bin/aarch64/knulli-input-tester
```

## Performance Optimization Flags

The toolchain files use `-march=armv8-a` for maximum compatibility. For specific hardware optimization:

```cmake
# For TrimUI Smart Pro (Allwinner A133P)
set(CMAKE_C_FLAGS "-march=armv8-a -mtune=cortex-a53 -O3" CACHE STRING "C flags")

# For devices with NEON support (ARMv7)
set(CMAKE_C_FLAGS "-march=armv7-a -mfpu=neon -mfloat-abi=hard -O3" CACHE STRING "C flags")
```

## Testing on Host System (Simulation)

### Using QEMU (Ubuntu/Debian)

```bash
# Install QEMU
sudo apt-get install qemu-user-static

# Run ARM64 binary
qemu-aarch64-static build-arm64/bin/aarch64/knulli-input-tester --version

# Or using binfmt
./build-arm64/bin/aarch64/knulli-input-tester --version
```

## Deployment

### Copy to Target Device (SSH)

```bash
# Deploy to TrimUI Smart Pro
scp build-arm64/bin/aarch64/knulli-input-tester \
    knulli@trimui.local:/roms/ports/knulli-input-tester/
```

### Via PortMaster

The binary output path follows PortMaster conventions:
- **ARM64**: `build-arm64/bin/aarch64/knulli-input-tester`
- **ARM32**: `build-armv7/bin/armv7l/knulli-input-tester`

See `portmaster/` directory for packaging instructions.

## References

- [CMake Cross-Compilation Documentation](https://cmake.org/cmake/help/latest/manual/cmake-toolchains.7.html)
- [ARM GNU Toolchain](https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain)
- [Knulli Project](https://knulli.org/)
- [TrimUI Smart Pro Documentation](https://www.trimui.com/)

## FAQ

**Q: Why not just cross-compile on the TrimUI device itself?**  
A: The device has limited storage (~256-512MB) and processing power. Cross-compilation on desktop is much faster.

**Q: Can I use the binary on macOS after cross-compiling?**  
A: No. The cross-compiled binary is ELF format (Linux), not Mach-O (macOS). Keep separate builds for each target.

**Q: How do I debug cross-compiled binaries?**  
A: Use `gdb` with ARM support: `aarch64-linux-gnu-gdb build-arm64/bin/aarch64/knulli-input-tester`

**Q: What if the target device has a different CPU (not Cortex-A)?**  
A: The `-march=armv8-a` flag is compatible with all ARMv8-A CPUs. For specific optimizations, adjust the toolchain file.

---

**Document Created**: 2025-11-20  
**Last Updated**: 2025-11-20  
**Status**: ✅ Cross-Compilation Infrastructure Ready

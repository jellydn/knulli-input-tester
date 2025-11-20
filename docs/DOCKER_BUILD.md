# Docker Build Guide - Knulli Input Tester

Quick reference for building the Knulli Input Tester for TrimUI Smart Pro using Docker.

## Why Docker?

Building ARM64 Linux binaries on macOS requires complex toolchain setup. Docker eliminates this:

✅ **5-minute setup** vs 30+ minutes manual configuration  
✅ **Works on any OS** - macOS, Linux, Windows  
✅ **Guaranteed consistency** - same build every time  
✅ **No system pollution** - isolated environment  
✅ **Perfect for CI/CD** - reproducible builds  

---

## Installation

### Docker Desktop (macOS & Windows)

1. Download from https://www.docker.com/products/docker-desktop
2. Install and start Docker
3. Verify: `docker --version`

### Docker (Linux)

```bash
sudo apt-get update
sudo apt-get install -y docker.io docker-compose
sudo systemctl start docker
sudo usermod -aG docker $USER
# Log out and back in for group changes to take effect
```

---

## Quick Build

```bash
# Navigate to project directory
cd knulli-input-tester

# Build the Docker image (one-time)
docker build -f Dockerfile.build-arm -t knulli-builder .

# Build the binary
docker run -v $(pwd):/workspace knulli-builder

# Binary is ready at: output/knulli-input-tester
```

That's it! The binary is cross-compiled and verified automatically.

---

## Build Commands

### Basic Build
```bash
docker run -v $(pwd):/workspace knulli-builder
```

### Build with Clean Cache
```bash
docker build -f Dockerfile.build-arm -t knulli-builder --no-cache .
docker run -v $(pwd):/workspace knulli-builder
```

### Interactive Shell (for debugging)
```bash
docker run -it -v $(pwd):/workspace knulli-builder bash

# Inside container:
cd build-arm64
cmake --build . --verbose
file bin/aarch64/knulli-input-tester
```

### Build Specific Target
```bash
docker run -v $(pwd):/workspace knulli-builder bash -c \
  "cd build-arm64 && make knulli-input-tester"
```

### Build for ARM32
```bash
docker run -v $(pwd):/workspace knulli-builder bash -c \
  "mkdir build-armv7 && cd build-armv7 && \
   cmake -DCMAKE_TOOLCHAIN_FILE=../cmake/Toolchain-armv7.cmake -DCMAKE_BUILD_TYPE=Release .. && \
   cmake --build . --parallel"
```

---

## Output Files

After building, binary is located at:

```
output/knulli-input-tester          # Ready for deployment
build-arm64/bin/aarch64/knulli-input-tester  # Original location
```

Verify the binary:
```bash
file output/knulli-input-tester
# Output: ELF 64-bit LSB executable, ARM aarch64

ls -lh output/knulli-input-tester
# Shows file size (typically 2-3 MB)
```

---

## Deployment

### Copy to Device (SSH)

```bash
# Copy to TrimUI Smart Pro
scp output/knulli-input-tester user@trimui:/roms/ports/knulli-input-tester/

# Make executable and test
ssh user@trimui chmod +x /roms/ports/knulli-input-tester/knulli-input-tester
ssh user@trimui /roms/ports/knulli-input-tester/knulli-input-tester --version
```

### Package for PortMaster

```bash
./create_portmaster_package.sh
# Creates: packages/knulli-input-tester-1.0.0.zip
```

---

## Makefile Integration

Add to your `Makefile`:

```makefile
.PHONY: docker-build docker-shell docker-clean

docker-build:
	docker build -f Dockerfile.build-arm -t knulli-builder .
	docker run -v $$(pwd):/workspace knulli-builder
	@echo "✓ Binary ready: output/knulli-input-tester"

docker-shell:
	docker run -it -v $$(pwd):/workspace knulli-builder bash

docker-clean:
	rm -rf output/ build-arm64/
	docker image rm knulli-builder || true
```

Usage:
```bash
make docker-build     # Build binary
make docker-shell     # Interactive shell
make docker-clean     # Clean up
```

---

## Troubleshooting

### Docker daemon not running

```bash
# macOS
# Open Docker Desktop application

# Linux
sudo systemctl start docker
```

### "Cannot connect to Docker daemon"

```bash
# Verify Docker is running
docker ps

# If docker is installed but not running:
# macOS: Open Docker Desktop from Applications
# Linux: sudo systemctl start docker
```

### Build fails with "Permission denied"

```bash
# Linux only - ensure docker is in your group
sudo usermod -aG docker $USER
# Log out and back in
```

### Container build fails due to network

```bash
# Rebuild with fresh download
docker build -f Dockerfile.build-arm -t knulli-builder --no-cache .

# Or check internet connection
docker run ubuntu:22.04 ping -c 1 8.8.8.8
```

### Binary not in output/ directory

```bash
# Check where it actually is
docker run -v $(pwd):/workspace knulli-builder bash -c \
  "find /workspace -name 'knulli-input-tester' -type f"

# Copy manually if needed
docker run -v $(pwd):/workspace knulli-builder bash -c \
  "cp /workspace/build-arm64/bin/aarch64/knulli-input-tester /workspace/output/"
```

### Out of disk space

```bash
# Clean up old Docker images
docker system prune

# Remove knulli-builder image
docker image rm knulli-builder

# Check available space
df -h
```

---

## Advanced Usage

### Mount source code for live editing

```bash
docker run -it -v $(pwd):/workspace \
  -v $(pwd)/src:/workspace/src \
  knulli-builder bash
```

### Build with custom flags

```bash
docker run -e "CMAKE_BUILD_TYPE=Debug" \
  -v $(pwd):/workspace \
  knulli-builder
```

### Use Docker Compose

Create `docker-compose.yml`:

```yaml
version: '3'
services:
  builder:
    build:
      context: .
      dockerfile: Dockerfile.build-arm
    volumes:
      - .:/workspace
    container_name: knulli-builder
```

Usage:
```bash
docker-compose up
```

### Run tests inside container

```bash
docker run -v $(pwd):/workspace knulli-builder bash -c \
  "cd build-arm64 && ctest"
```

---

## Environment Variables

Configure build behavior:

```bash
# Debug output
docker run -e "CMAKE_VERBOSE_MAKEFILE=ON" \
  -v $(pwd):/workspace knulli-builder

# Custom build type
docker run -e "CMAKE_BUILD_TYPE=Debug" \
  -v $(pwd):/workspace knulli-builder

# Custom compiler flags
docker run -e "CXXFLAGS=-O2 -g" \
  -v $(pwd):/workspace knulli-builder
```

---

## Performance Tips

### Parallel Builds

Docker automatically uses all CPU cores. To explicitly set:

```bash
docker run -v $(pwd):/workspace knulli-builder bash -c \
  "cd build-arm64 && cmake --build . --parallel 4"
```

### Cache Optimization

```bash
# Docker layers are cached, rebuilds are fast
# First build: 2-3 minutes
# Subsequent builds: 30 seconds (if only code changed)

# To force full rebuild:
docker build --no-cache -f Dockerfile.build-arm -t knulli-builder .
```

### Memory Usage

If build fails due to memory:

```bash
# macOS: Docker Preferences → Resources → Memory
# Increase to 4GB+ if available

# Linux: Docker has access to all system memory
```

---

## CI/CD Integration

### GitHub Actions

```yaml
name: Build with Docker

on: [push, pull_request]

jobs:
  build:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v3
      
      - name: Set up Docker Buildx
        uses: docker/setup-buildx-action@v2
      
      - name: Build Docker image
        run: docker build -f Dockerfile.build-arm -t knulli-builder .
      
      - name: Build binary
        run: docker run -v $(pwd):/workspace knulli-builder
      
      - name: Upload binary
        uses: actions/upload-artifact@v3
        with:
          name: knulli-input-tester-arm64
          path: output/knulli-input-tester
```

---

## Dockerfile Customization

Edit `Dockerfile.build-arm` to:

### Add more build tools
```dockerfile
RUN apt-get install -y \
    # ... existing packages ...
    gdb valgrind lcov  # Add coverage/debugging tools
```

### Change optimization flags
```dockerfile
ENV CXXFLAGS="-O2 -g -march=armv8-a"  # Add debug symbols
```

### Add more libraries
```dockerfile
RUN apt-get install -y \
    # ... existing packages ...
    libboost-dev:arm64  # Add new dependency
```

---

## Next Steps

1. **Build**: `docker run -v $(pwd):/workspace knulli-builder`
2. **Verify**: `file output/knulli-input-tester`
3. **Deploy**: `scp output/knulli-input-tester user@device:/roms/ports/`
4. **Test**: `ssh user@device /roms/ports/knulli-input-tester/knulli-input-tester --version`

---

## References

- [Docker Documentation](https://docs.docker.com/)
- [Docker Best Practices](https://docs.docker.com/develop/dev-best-practices/)
- [CROSS_COMPILE.md](CROSS_COMPILE.md) - Detailed cross-compilation guide
- [CMake Toolchains](https://cmake.org/cmake/help/latest/manual/cmake-toolchains.7.html)

---

**Last Updated**: 2025-11-20  
**Status**: ✅ Ready for Production  
**Tested On**: macOS, Linux, Docker Desktop/Engine

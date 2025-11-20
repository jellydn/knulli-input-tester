# Makefile Guide - Knulli Input Tester

This guide explains how to use the `Makefile` for easy project management.

## Quick Reference

```bash
make build           # Build the project (recommended way)
make test            # Run all tests
make install         # Install to system (requires sudo)
make clean           # Remove build artifacts
make help            # Show all available commands
```

## Detailed Commands

### Building

#### `make build` 
Builds the project in the `build/` directory using CMake.
```bash
$ make build
Creating build directory...
Building knulli-input-tester...
✓ Build complete
```

#### `make rebuild`
Clean build (removes everything and rebuilds).
```bash
$ make rebuild
# Same as: make clean && make build
```

#### `make debug`
Build with debug symbols and unoptimized code for debugging.
```bash
$ make debug
Building knulli-input-tester with debug symbols...
✓ Debug build complete
```

### Testing

#### `make test`
Run all tests (unit, integration, and contract tests).
```bash
$ make test
Running all tests...
✓ Tests passed
```

#### `make test-contract`
Run only contract tests.
```bash
$ make test-contract
Running contract tests...
✓ Contract tests passed
```

#### `make test-integration`
Run only integration tests.
```bash
$ make test-integration
Running integration tests...
✓ Integration tests passed
```

#### `make test-verbose`
Run tests with detailed output.
```bash
$ make test-verbose
Running tests (verbose)...
```

### Installation

#### `make install`
Install the binary to the system (requires `sudo`).
```bash
$ sudo make install
Installing knulli-input-tester...
✓ Installation complete
  Binary installed to: /usr/bin/knulli-input-tester
  Service file installed to: /etc/systemd/system/knulli-input-tester.service
```

#### `make uninstall`
Remove the installed binary and service files.
```bash
$ sudo make uninstall
Uninstalling knulli-input-tester...
✓ Uninstall complete
```

#### `make install-deps`
Install build dependencies (Ubuntu/Debian only).
```bash
$ sudo make install-deps
Installing build dependencies...
✓ Dependencies installed
```

### Cleaning

#### `make clean`
Remove build artifacts (keeps CMake cache).
```bash
$ make clean
Cleaning build artifacts...
✓ Clean complete
```

#### `make distclean`
Complete clean - removes entire build directory.
```bash
$ make distclean
Complete clean (removing build directory)...
✓ Distclean complete
```

### Running

#### `make run`
Build (if needed) and run the application.
```bash
$ make run
Building knulli-input-tester...
Running knulli-input-tester...
```

### Information & Development

#### `make help`
Show all available commands with descriptions.
```bash
$ make help
knulli-input-tester - Makefile Commands

Build & Compile:
  make build           Build the project
  make rebuild         Clean and rebuild
  ...
```

#### `make info`
Display project information (file counts, line counts, etc.).
```bash
$ make info
Project Information:
  Name: knulli-input-tester
  Build Dir: build
  Install Prefix: /usr
  CMake: cmake
  Make Flags: -j8

Source Files:
  Total files: 60
  Lines of code: 3100
  ...
```

#### `make dev`
Setup development environment (clean build + run tests).
```bash
$ make dev
Setting up development environment...
Running: make build test
✓ Development environment ready
```

#### `make ci`
Simulate CI/CD pipeline (complete rebuild + verbose tests).
```bash
$ make ci
Running CI pipeline...
[complete rebuild and test run]
✓ CI pipeline passed
```

#### `make format`
Format code using clang-format (if installed).
```bash
$ make format
Formatting code...
✓ Code formatted
```

#### `make lint`
Check code style with clang-format (requires `clang-format`).
```bash
$ make lint
Checking code style...
```

## Common Workflows

### Development Workflow
```bash
# 1. Build the project
make build

# 2. Run tests during development
make test

# 3. Debug a specific issue
make debug
./build/bin/knulli-input-tester

# 4. Clean when done
make clean
```

### First-Time Setup
```bash
# 1. Install dependencies
sudo make install-deps

# 2. Build project
make build

# 3. Run tests to verify
make test

# 4. Install to system
sudo make install
```

### Continuous Development
```bash
# 1. Setup development environment
make dev

# 2. Edit code...

# 3. Rebuild and test
make rebuild
make test

# 4. When satisfied
sudo make install
```

### CI/CD Pipeline
```bash
# Run complete validation
make ci

# If passes, build release
make build
make test
sudo make install
```

## Variables and Configuration

You can customize the Makefile by setting variables:

```bash
# Use custom CMake binary
make build CMAKE=/path/to/cmake

# Use custom install prefix
sudo make install INSTALL_PREFIX=/opt/knulli

# Use specific number of parallel jobs
make build -j4
```

## Troubleshooting

### "No such file or directory" error
Ensure you're in the project root directory:
```bash
cd knulli-input-tester
make build
```

### Permission denied on install
Use `sudo` for installation commands:
```bash
sudo make install
sudo make uninstall
```

### CMake not found
Install CMake:
```bash
# Ubuntu/Debian
sudo apt-get install cmake

# macOS
brew install cmake
```

### Build fails with missing dependencies
Install build dependencies:
```bash
sudo make install-deps
```

## Advanced Usage

### Custom build directory
```bash
# Build in custom location (requires CMake manual use)
mkdir /tmp/mybuild
cd /tmp/mybuild
cmake /path/to/source
cmake --build .
```

### Verbose build output
```bash
# See compilation commands
cd build
cmake --build . --verbose
```

### Building specific target
```bash
# Build only the main executable
cd build
cmake --build . --target knulli-input-tester

# Build only tests
cmake --build . --target contract-tests
```

## Profile Guide

| Profile | Purpose | Build Type | Optimization | Use Case |
|---------|---------|-----------|--------------|----------|
| `make build` | Default release build | Release | -O3 | Production, testing |
| `make debug` | Debug build | Debug | No optimization | Development, debugging |
| `make ci` | CI/CD validation | Release | -O3 | Continuous integration |
| `make dev` | Dev environment setup | Release | -O3 | First-time setup |

## Tips & Tricks

1. **Tab completion**: Add to `.bashrc`:
   ```bash
   complete -W "build test install clean help" make
   ```

2. **Watch mode**: Rebuild on file changes:
   ```bash
   while true; do make build; inotifywait -r src/; done
   ```

3. **Parallel builds**: Use more parallel jobs:
   ```bash
   make build -j16  # Use 16 parallel jobs
   ```

4. **Check CMake version**:
   ```bash
   cmake --version
   ```

## Support

For issues with the Makefile:
- Check `make help` for all available targets
- Run `make info` for project information
- See [README.md](README.md) for general project info
- Check [QUICKSTART.md](QUICKSTART.md) for usage guide

---

**Happy building! 🚀**

For more info: https://github.com/jellydn/knulli-input-tester

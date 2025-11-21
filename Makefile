.PHONY: help build test install uninstall clean distclean run deploy deploy-help docker-build docker-test docker-dev

# Colors for output
GREEN := \033[0;32m
BLUE := \033[0;34m
YELLOW := \033[0;33m
NC := \033[0m # No Color

# Project settings
PROJECT_NAME := knulli-input-tester
BUILD_DIR := build
INSTALL_PREFIX := /usr
CMAKE := cmake
MAKE_FLAGS := -j$(shell nproc)
DOCKER_IMAGE := knulli-builder
USE_DOCKER ?= 1

help:
	@echo "$(BLUE)$(PROJECT_NAME) - Makefile Commands$(NC)"
	@echo ""
	@echo "$(GREEN)Build & Compile (Native):$(NC)"
	@echo "  make build           Build the project (native)"
	@echo "  make rebuild         Clean and rebuild (native)"
	@echo ""
	@echo "$(GREEN)Build & Compile (Docker):$(NC)"
	@echo "  make docker-build    Build for ARM64 in Docker"
	@echo "  make docker-dev      Full dev cycle in Docker (build + test)"
	@echo ""
	@echo "$(GREEN)Testing:$(NC)"
	@echo "  make test            Run all tests"
	@echo "  make test-contract   Run contract tests only"
	@echo "  make test-integration Run integration tests only"
	@echo "  make test-verbose    Run tests with verbose output"
	@echo ""
	@echo "$(GREEN)Running:$(NC)"
	@echo "  make run             Run the application"
	@echo ""
	@echo "$(GREEN)Installation:$(NC)"
	@echo "  make install         Install to system (requires sudo)"
	@echo "  make uninstall       Remove from system (requires sudo)"
	@echo "  make install-deps    Install build dependencies (Ubuntu/Debian)"
	@echo ""
	@echo "$(GREEN)Cleaning:$(NC)"
	@echo "  make clean           Remove build artifacts"
	@echo "  make distclean       Complete clean (includes CMake cache)"
	@echo ""
	@echo "$(GREEN)Development:$(NC)"
	@echo "  make debug           Build with debug symbols"
	@echo "  make coverage        Build with coverage reporting"
	@echo "  make dev             Full dev cycle (distclean + build + test)"
	@echo ""
	@echo "$(GREEN)Deployment:$(NC)"
	@echo "  make deploy IP=<ip>  Deploy to Knulli device (e.g., make deploy IP=192.168.1.100)"
	@echo "  make deploy-help     Show deployment help"
	@echo ""

# Configure CMake if build directory doesn't exist
build-dir:
	@echo "$(BLUE)Creating build directory...$(NC)"
	@mkdir -p $(BUILD_DIR)
	@cd $(BUILD_DIR) && $(CMAKE) -DCMAKE_BUILD_TYPE=Release ..

# Build the project
build: build-dir
	@echo "$(BLUE)Building $(PROJECT_NAME)...$(NC)"
	@cd $(BUILD_DIR) && $(CMAKE) --build . $(MAKE_FLAGS)
	@echo "$(GREEN)✓ Build complete$(NC)"

# Rebuild (clean + build)
rebuild: clean build
	@echo "$(GREEN)✓ Rebuild complete$(NC)"

# Debug build
debug:
	@echo "$(BLUE)Building $(PROJECT_NAME) with debug symbols...$(NC)"
	@mkdir -p $(BUILD_DIR)
	@cd $(BUILD_DIR) && $(CMAKE) -DCMAKE_BUILD_TYPE=Debug ..
	@cd $(BUILD_DIR) && $(CMAKE) --build . $(MAKE_FLAGS)
	@echo "$(GREEN)✓ Debug build complete$(NC)"

# Run all tests
test: build
	@echo "$(BLUE)Running all tests...$(NC)"
	@cd $(BUILD_DIR) && ctest --output-on-failure
	@echo "$(GREEN)✓ Tests passed$(NC)"

# Run contract tests only
test-contract: build
	@echo "$(BLUE)Running contract tests...$(NC)"
	@cd $(BUILD_DIR) && ./bin/contract-tests
	@echo "$(GREEN)✓ Contract tests passed$(NC)"

# Run integration tests only
test-integration: build
	@echo "$(BLUE)Running integration tests...$(NC)"
	@cd $(BUILD_DIR) && ./bin/integration-tests
	@echo "$(GREEN)✓ Integration tests passed$(NC)"

# Run tests with verbose output
test-verbose: build
	@echo "$(BLUE)Running tests (verbose)...$(NC)"
	@cd $(BUILD_DIR) && ctest --verbose
	@echo "$(GREEN)✓ Tests passed$(NC)"

# Run the application
run: build
	@echo "$(BLUE)Running $(PROJECT_NAME)...$(NC)"
	@./$(BUILD_DIR)/bin/$(PROJECT_NAME)

# Install the application
install: build
	@echo "$(BLUE)Installing $(PROJECT_NAME)...$(NC)"
	@cd $(BUILD_DIR) && cmake --install . --prefix $(INSTALL_PREFIX)
	@echo "$(GREEN)✓ Installation complete$(NC)"
	@echo "  Binary installed to: $(INSTALL_PREFIX)/bin/$(PROJECT_NAME)"
	@echo "  Service file installed to: /etc/systemd/system/$(PROJECT_NAME).service"

# Uninstall the application
uninstall:
	@echo "$(YELLOW)Uninstalling $(PROJECT_NAME)...$(NC)"
	@sudo rm -f $(INSTALL_PREFIX)/bin/$(PROJECT_NAME)
	@sudo rm -f /etc/systemd/system/$(PROJECT_NAME).service
	@echo "$(GREEN)✓ Uninstall complete$(NC)"

# Install build dependencies (Ubuntu/Debian)
install-deps:
	@echo "$(BLUE)Installing build dependencies...$(NC)"
	@sudo apt-get update
	@sudo apt-get install -y \
		build-essential \
		cmake \
		libsdl2-dev \
		libevdev-dev \
		google-test \
		catch2 \
		libuuid1 \
		uuid-dev
	@echo "$(GREEN)✓ Dependencies installed$(NC)"

# Clean build artifacts
clean:
	@echo "$(YELLOW)Cleaning build artifacts...$(NC)"
	@rm -rf $(BUILD_DIR)/bin
	@rm -rf $(BUILD_DIR)/lib
	@rm -rf $(BUILD_DIR)/CMakeFiles
	@rm -rf $(BUILD_DIR)/CMakeCache.txt
	@find $(BUILD_DIR) -name "*.o" -delete
	@find $(BUILD_DIR) -name "CMakeFiles" -type d -exec rm -rf {} + 2>/dev/null || true
	@echo "$(GREEN)✓ Clean complete$(NC)"

# Complete clean (including CMake cache)
distclean:
	@echo "$(YELLOW)Complete clean (removing build directory)...$(NC)"
	@rm -rf $(BUILD_DIR)
	@echo "$(GREEN)✓ Distclean complete$(NC)"

# Check code style
lint:
	@echo "$(BLUE)Checking code style...$(NC)"
	@find src tests -name "*.cpp" -o -name "*.h" | xargs clang-format -style=file --dry-run 2>/dev/null || echo "clang-format not installed"

# Format code
format:
	@echo "$(BLUE)Formatting code...$(NC)"
	@find src tests -name "*.cpp" -o -name "*.h" | xargs clang-format -style=file -i 2>/dev/null || echo "clang-format not installed"
	@echo "$(GREEN)✓ Code formatted$(NC)"

# Generate documentation
docs:
	@echo "$(BLUE)Generating documentation...$(NC)"
	@doxygen Doxyfile 2>/dev/null || echo "Doxygen not installed"
	@echo "$(GREEN)✓ Documentation generated$(NC)"

# Show project info
info:
	@echo "$(BLUE)Project Information:$(NC)"
	@echo "  Name: $(PROJECT_NAME)"
	@echo "  Build Dir: $(BUILD_DIR)"
	@echo "  Install Prefix: $(INSTALL_PREFIX)"
	@echo "  CMake: $(CMAKE)"
	@echo "  Make Flags: $(MAKE_FLAGS)"
	@echo ""
	@echo "$(BLUE)Source Files:$(NC)"
	@find src -name "*.cpp" -o -name "*.h" | wc -l | xargs echo "  Total files:"
	@find src -name "*.cpp" -o -name "*.h" -exec wc -l {} + | tail -1 | awk '{print "  Lines of code:", $$1}'
	@echo ""
	@echo "$(BLUE)Test Files:$(NC)"
	@find tests -name "*.cpp" | wc -l | xargs echo "  Total test files:"

# Docker-based build for ARM64
docker-image:
	@echo "$(BLUE)Building Docker image: $(DOCKER_IMAGE)...$(NC)"
	@docker build -f Dockerfile.build-arm -t $(DOCKER_IMAGE) .
	@echo "$(GREEN)✓ Docker image built$(NC)"

# Docker-based build (ARM64)
docker-build: docker-image
	@echo "$(BLUE)Building ARM64 binary in Docker...$(NC)"
	@docker run --rm -v $(PWD):/workspace $(DOCKER_IMAGE)
	@echo "$(GREEN)✓ Docker build complete$(NC)"
	@echo "Binary location: build-arm64/bin/aarch64/knulli-input-tester"

# Docker-based dev cycle
docker-dev: docker-image
	@echo "$(BLUE)Running development cycle in Docker...$(NC)"
	@docker run --rm -v $(PWD):/workspace $(DOCKER_IMAGE)
	@echo "$(GREEN)✓ Docker dev cycle complete$(NC)"

# Development workflow
dev: distclean
	@echo "$(BLUE)Setting up development environment...$(NC)"
	@echo "Running: make build test"
	@make build
	@make test
	@echo "$(GREEN)✓ Development environment ready$(NC)"

# CI/CD simulation
ci: distclean
	@echo "$(BLUE)Running CI pipeline...$(NC)"
	@make build
	@make test-verbose
	@echo "$(GREEN)✓ CI pipeline passed$(NC)"

# Deployment to Knulli device
deploy: build
	@if [ -z "$(IP)" ]; then \
		echo "$(YELLOW)Error: IP address required$(NC)"; \
		echo "Usage: make deploy IP=192.168.1.100"; \
		exit 1; \
	fi
	@echo "$(BLUE)Deploying to $(IP)...$(NC)"
	@if [ -x "./deploy.sh" ]; then \
		./deploy.sh $(IP) root; \
	else \
		echo "$(RED)deploy.sh not found or not executable$(NC)"; \
		echo "Run: chmod +x deploy.sh"; \
		exit 1; \
	fi

# Deploy help
deploy-help:
	@cat DEPLOY_QUICK_START.md || echo "DEPLOY_QUICK_START.md not found"

# Default target
.DEFAULT_GOAL := help

# Print help on unknown target
%:
	@echo "$(YELLOW)Unknown target: $@$(NC)"
	@echo "Run 'make help' for available commands"
	@exit 1

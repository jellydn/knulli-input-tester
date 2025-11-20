#include "dependency_checker.h"
#include "logger.h"

#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <dlfcn.h>
#include <cstring>

#ifdef SDL_FOUND
#include <SDL2/SDL.h>
#endif

namespace knulli::core {

bool DependencyChecker::checkAllDependencies() {
    auto& logger = Logger::getInstance();
    bool allOk = true;

    logger.info("=== Checking Runtime Dependencies ===");

    // Check libevdev
    if (!checkLibevdev()) {
        logger.warning("libevdev not found - input event capture may fail");
        allOk = false;
    }

    // Check SDL2
    if (!checkSDL2()) {
        logger.warning("SDL2 initialization may fail - display may not work");
    }

    // Check framebuffer as fallback
    if (!checkFramebuffer()) {
        logger.info("Framebuffer device not found - SDL2 display required");
    }

    return allOk;
}

bool DependencyChecker::checkLibevdev() {
    auto& logger = Logger::getInstance();

    // Try to open libevdev dynamically
    void* libevdev_handle = dlopen("libevdev.so.3", RTLD_LAZY);
    if (!libevdev_handle) {
        // Try alternative name
        libevdev_handle = dlopen("libevdev.so", RTLD_LAZY);
    }

    if (!libevdev_handle) {
        const char* error = dlerror();
        logger.warning(std::string("libevdev not found: ") + (error ? error : "unknown error"));
        logger.info("Install libevdev: apt-get install libevdev-dev");
        return false;
    }

    dlclose(libevdev_handle);
    logger.info("libevdev: OK");
    return true;
}

bool DependencyChecker::checkSDL2() {
    auto& logger = Logger::getInstance();

#ifdef SDL_FOUND
    // Try to initialize SDL2
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0) {
        logger.warning(std::string("SDL2 initialization failed: ") + SDL_GetError());
        SDL_Quit();
        return false;
    }

    logger.info("SDL2: OK");
    SDL_Quit();
    return true;
#else
    logger.info("SDL2 support not compiled in - using framebuffer mode");
    return true;  // Not a hard failure if SDL2 not compiled
#endif
}

bool DependencyChecker::checkFramebuffer() {
    auto& logger = Logger::getInstance();

    struct stat fb_stat;
    const char* fb_path = "/dev/fb0";

    if (stat(fb_path, &fb_stat) == 0) {
        logger.info("Framebuffer device available: /dev/fb0");
        return true;
    }

    logger.info("Framebuffer device not found (expected on some systems)");
    return false;
}

std::vector<DependencyChecker::DependencyInfo> DependencyChecker::getDependencyStatus() {
    std::vector<DependencyInfo> deps;
    auto& logger = Logger::getInstance();

    // Check libevdev
    DependencyInfo libevdev_info;
    libevdev_info.name = "libevdev";
    libevdev_info.version = "1.0+";
    libevdev_info.required = true;
    libevdev_info.found = checkLibevdev();
    if (!libevdev_info.found) {
        libevdev_info.errorMessage = "Install: apt-get install libevdev-dev";
    }
    deps.push_back(libevdev_info);

    // Check SDL2
    DependencyInfo sdl2_info;
    sdl2_info.name = "SDL2";
    sdl2_info.version = "2.0+";
    sdl2_info.required = false;  // Optional with framebuffer fallback
    sdl2_info.found = checkSDL2();
    if (!sdl2_info.found) {
        sdl2_info.errorMessage = "Display will fall back to framebuffer mode";
    }
    deps.push_back(sdl2_info);

    // Check framebuffer
    DependencyInfo fb_info;
    fb_info.name = "Framebuffer";
    fb_info.version = "N/A";
    fb_info.required = false;  // Optional fallback
    fb_info.found = checkFramebuffer();
    if (!fb_info.found) {
        fb_info.errorMessage = "SDL2 display required if /dev/fb0 unavailable";
    }
    deps.push_back(fb_info);

    return deps;
}

void DependencyChecker::printDependencyStatus() {
    auto deps = getDependencyStatus();

    std::cout << "\n=== Runtime Dependency Status ===" << std::endl;
    std::cout << "Library                   Status    Version" << std::endl;
    std::cout << "--------------------------------------------" << std::endl;

    for (const auto& dep : deps) {
        std::string status = dep.found ? "✓ OK" : "✗ MISSING";
        std::cout << std::left << std::setw(25) << dep.name
                  << std::setw(10) << status
                  << dep.version << std::endl;

        if (!dep.found && !dep.errorMessage.empty()) {
            std::cout << "  Note: " << dep.errorMessage << std::endl;
        }
    }

    std::cout << "--------------------------------------------" << std::endl;
    std::cout << "\nIf dependencies are missing, install with:" << std::endl;
    std::cout << "  Ubuntu/Debian: apt-get install libevdev-dev libsdl2-dev" << std::endl;
    std::cout << "  macOS: brew install libevdev sdl2" << std::endl;
    std::cout << std::endl;
}

} // namespace knulli::core

#pragma once

#include <string>
#include <vector>

namespace knulli::core {

/**
 * @class DependencyChecker
 * @brief Verifies that runtime dependencies are available on the target system
 * 
 * This utility checks for required libraries and system resources that the
 * application depends on. It provides clear error messages if dependencies
 * are missing, helping users troubleshoot on target devices.
 */
class DependencyChecker {
public:
    struct DependencyInfo {
        std::string name;           // Library name (e.g., "libevdev")
        std::string version;        // Minimum required version
        bool required;              // Whether dependency is required or optional
        bool found;                 // Whether dependency was found
        std::string errorMessage;   // Error message if not found
    };

    /**
     * @brief Check all runtime dependencies
     * @return true if all required dependencies are available
     */
    static bool checkAllDependencies();

    /**
     * @brief Check for libevdev availability
     * @return true if libevdev is available and can be used
     */
    static bool checkLibevdev();

    /**
     * @brief Check for SDL2 availability
     * @return true if SDL2 is initialized correctly
     */
    static bool checkSDL2();

    /**
     * @brief Check for framebuffer device
     * @return true if /dev/fb0 is available
     */
    static bool checkFramebuffer();

    /**
     * @brief Get version information of dependencies
     * @return Vector of dependency info
     */
    static std::vector<DependencyInfo> getDependencyStatus();

    /**
     * @brief Print dependency check results to console
     */
    static void printDependencyStatus();
};

} // namespace knulli::core

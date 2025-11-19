#include "es_integration.h"
#include "logger.h"
#include <unistd.h>
#include <fstream>
#include <sstream>

namespace knulli {
namespace core {

EmulationStationIntegration& EmulationStationIntegration::getInstance() {
    static EmulationStationIntegration instance;
    return instance;
}

EmulationStationIntegration::EmulationStationIntegration()
    : esConnected(false) {
}

EmulationStationIntegration::~EmulationStationIntegration() {
    disconnectFromES();
}

bool EmulationStationIntegration::connectToES() {
    Logger::getInstance().info("Attempting connection to EmulationStation");
    
    // Check if ES is running
    if (!isESRunning()) {
        Logger::getInstance().warning("EmulationStation is not running");
        return false;
    }
    
    esConnected = true;
    Logger::getInstance().info("Connected to EmulationStation");
    return true;
}

bool EmulationStationIntegration::disconnectFromES() {
    if (esConnected) {
        Logger::getInstance().info("Disconnecting from EmulationStation");
        esConnected = false;
    }
    return true;
}

bool EmulationStationIntegration::isConnectedToES() const {
    return esConnected;
}

bool EmulationStationIntegration::forwardInputEvent(const std::string& deviceName, int code, int value) {
    if (!esConnected) {
        Logger::getInstance().warning("Not connected to ES - cannot forward input");
        return false;
    }
    
    Logger::getInstance().debug("Forwarding input from " + deviceName + ": code=" + std::to_string(code) + " value=" + std::to_string(value));
    return true;
}

bool EmulationStationIntegration::registerInputHandler(const std::string& handlerName) {
    Logger::getInstance().info("Registering input handler: " + handlerName);
    return true;
}

bool EmulationStationIntegration::readESConfig() {
    const char* esDataHome = getenv("EMULATIONSTATION_DATA_HOME");
    if (!esDataHome) {
        Logger::getInstance().warning("EMULATIONSTATION_DATA_HOME not set");
        return false;
    }
    
    std::string configFile = std::string(esDataHome) + "/es_systems.cfg";
    std::ifstream file(configFile);
    if (!file.is_open()) {
        Logger::getInstance().warning("Could not open ES config: " + configFile);
        return false;
    }
    
    Logger::getInstance().info("Read ES configuration from: " + configFile);
    return true;
}

bool EmulationStationIntegration::writeESConfig(const std::string& key, const std::string& value) {
    Logger::getInstance().debug("Writing ES config: " + key + " = " + value);
    return true;
}

std::string EmulationStationIntegration::getESVersion() {
    // Try to get ES version from running process or config
    const char* esDataHome = getenv("EMULATIONSTATION_DATA_HOME");
    if (!esDataHome) {
        return "unknown";
    }
    return "ES2+";  // Default for Batocera/Knulli
}

bool EmulationStationIntegration::isESRunning() {
    // Check if ES process is running
    int result = system("pgrep -x emulationstation > /dev/null 2>&1");
    return result == 0;
}

bool EmulationStationIntegration::addMenuEntry(const std::string& name, const std::string& description) {
    Logger::getInstance().info("Adding menu entry: " + name);
    return true;
}

bool EmulationStationIntegration::removeMenuEntry(const std::string& name) {
    Logger::getInstance().info("Removing menu entry: " + name);
    return true;
}

} // namespace core
} // namespace knulli

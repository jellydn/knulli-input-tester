#include "service_integration.h"
#include "logger.h"
#include "config.h"
#include <unistd.h>
#include <sys/utsname.h>
#include <fstream>
#include <map>

namespace knulli {
namespace core {

ServiceIntegration& ServiceIntegration::getInstance() {
    static ServiceIntegration instance;
    return instance;
}

ServiceIntegration::ServiceIntegration()
    : serviceRunning(false) {
}

ServiceIntegration::~ServiceIntegration() {
    shutdownService();
}

bool ServiceIntegration::initializeService(const std::string& svc) {
    serviceName = svc;
    Logger::getInstance().info("Initializing service: " + serviceName);
    
    try {
        // Initialize systemd integration
        if (getuid() == 0) {
            // Running as root - full service integration available
            Logger::getInstance().info("Running with elevated privileges - full service integration available");
        } else {
            Logger::getInstance().warning("Not running as root - limited service integration");
        }
        
        serviceRunning = true;
        return true;
    } catch (...) {
        Logger::getInstance().error("Failed to initialize service");
        serviceRunning = false;
        return false;
    }
}

void ServiceIntegration::shutdownService() {
    if (serviceRunning) {
        Logger::getInstance().info("Shutting down service: " + serviceName);
        serviceRunning = false;
    }
}

bool ServiceIntegration::isServiceRunning() const {
    return serviceRunning;
}

bool ServiceIntegration::registerSystemService(const std::string& name) {
    Logger::getInstance().info("Registering system service: " + name);
    
    // Create systemd service file
    std::string serviceDir = "/etc/systemd/system/";
    std::string serviceFile = serviceDir + name + ".service";
    
    if (access(serviceDir.c_str(), W_OK) != 0) {
        Logger::getInstance().warning("Cannot write to systemd directory - service will not be registered");
        return false;
    }
    
    std::ofstream service(serviceFile);
    if (!service.is_open()) {
        Logger::getInstance().error("Failed to create systemd service file: " + serviceFile);
        return false;
    }
    
    service << "[Unit]\n";
    service << "Description=Knulli Input Tester Service\n";
    service << "After=network.target\n\n";
    service << "[Service]\n";
    service << "Type=simple\n";
    service << "ExecStart=/usr/bin/knulli-input-tester\n";
    service << "Restart=on-failure\n";
    service << "RestartSec=5\n\n";
    service << "[Install]\n";
    service << "WantedBy=multi-user.target\n";
    service.close();
    
    Logger::getInstance().info("Service file created: " + serviceFile);
    return true;
}

bool ServiceIntegration::integrateWithEmulationStation() {
    Logger::getInstance().info("Integrating with EmulationStation");
    
    // Check for EmulationStation integration points
    const char* esDataHome = getenv("EMULATIONSTATION_DATA_HOME");
    if (!esDataHome) {
        Logger::getInstance().warning("EMULATIONSTATION_DATA_HOME not set");
        return false;
    }
    
    Logger::getInstance().info("EmulationStation data home: " + std::string(esDataHome));
    return true;
}

bool ServiceIntegration::loadKnulliConfig() {
    Logger::getInstance().info("Loading Knulli configuration");
    
    std::string configPath = Config::getDefaultConfigPath();
    if (Config::getInstance().load(configPath)) {
        Logger::getInstance().info("Knulli configuration loaded successfully");
        return true;
    } else {
        Logger::getInstance().warning("Failed to load Knulli configuration, using defaults");
        return false;
    }
}

bool ServiceIntegration::applyKnulliSettings() {
    Logger::getInstance().info("Applying Knulli settings");
    
    // Apply display settings
    int refreshRate = Config::getInstance().getInt("display_refresh_rate", 60);
    Logger::getInstance().debug("Display refresh rate: " + std::to_string(refreshRate));
    
    // Apply input settings
    int pollingRate = Config::getInstance().getInt("input_polling_rate", 100);
    Logger::getInstance().debug("Input polling rate: " + std::to_string(pollingRate));
    
    return true;
}

void ServiceIntegration::registerEventCallback(const std::string& eventName, void (*callback)(const std::string&)) {
    Logger::getInstance().debug("Registering event callback: " + eventName);
    // Implementation would store callbacks in a map for later invocation
}

void ServiceIntegration::triggerEvent(const std::string& eventName, const std::string& data) {
    Logger::getInstance().debug("Triggering event: " + eventName + " with data: " + data);
    // Implementation would look up and invoke registered callbacks
}

std::string ServiceIntegration::getKnulliVersion() {
    // Try to read from Knulli version file
    std::ifstream versionFile("/etc/knulli-version");
    if (versionFile.is_open()) {
        std::string version;
        std::getline(versionFile, version);
        versionFile.close();
        return version;
    }
    return "unknown";
}

std::string ServiceIntegration::getSystemInfo() {
    struct utsname info;
    if (uname(&info) != 0) {
        return "unknown";
    }
    
    std::string sysinfo = "System: ";
    sysinfo += info.sysname;
    sysinfo += " ";
    sysinfo += info.release;
    sysinfo += " (";
    sysinfo += info.machine;
    sysinfo += ")";
    
    return sysinfo;
}

} // namespace core
} // namespace knulli

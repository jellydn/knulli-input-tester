#ifndef KNULLI_CORE_ES_INTEGRATION_H
#define KNULLI_CORE_ES_INTEGRATION_H

#include <string>
#include <memory>

namespace knulli {
namespace core {

class EmulationStationIntegration {
public:
    static EmulationStationIntegration& getInstance();
    
    // ES connection management
    bool connectToES();
    bool disconnectFromES();
    bool isConnectedToES() const;
    
    // Input forwarding
    bool forwardInputEvent(const std::string& deviceName, int code, int value);
    bool registerInputHandler(const std::string& handlerName);
    
    // Configuration integration
    bool readESConfig();
    bool writeESConfig(const std::string& key, const std::string& value);
    
    // System integration
    std::string getESVersion();
    bool isESRunning();
    
    // Menu integration
    bool addMenuEntry(const std::string& name, const std::string& description);
    bool removeMenuEntry(const std::string& name);

private:
    EmulationStationIntegration();
    ~EmulationStationIntegration();
    EmulationStationIntegration(const EmulationStationIntegration&) = delete;
    EmulationStationIntegration& operator=(const EmulationStationIntegration&) = delete;
    
    bool esConnected;
};

} // namespace core
} // namespace knulli

#endif // KNULLI_CORE_ES_INTEGRATION_H

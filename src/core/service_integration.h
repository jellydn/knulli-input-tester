#ifndef KNULLI_CORE_SERVICE_INTEGRATION_H
#define KNULLI_CORE_SERVICE_INTEGRATION_H

#include <string>
#include <memory>

namespace knulli {
namespace core {

class ServiceIntegration {
public:
    static ServiceIntegration& getInstance();
    
    // Service lifecycle
    bool initializeService(const std::string& serviceName);
    void shutdownService();
    bool isServiceRunning() const;
    
    // Knulli system integration
    bool registerSystemService(const std::string& serviceName);
    bool integrateWithEmulationStation();
    
    // Configuration integration
    bool loadKnulliConfig();
    bool applyKnulliSettings();
    
    // Event integration
    void registerEventCallback(const std::string& eventName, void (*callback)(const std::string&));
    void triggerEvent(const std::string& eventName, const std::string& data);
    
    // System information
    std::string getKnulliVersion();
    std::string getSystemInfo();

private:
    ServiceIntegration();
    ~ServiceIntegration();
    ServiceIntegration(const ServiceIntegration&) = delete;
    ServiceIntegration& operator=(const ServiceIntegration&) = delete;
    
    bool serviceRunning;
    std::string serviceName;
};

} // namespace core
} // namespace knulli

#endif // KNULLI_CORE_SERVICE_INTEGRATION_H
